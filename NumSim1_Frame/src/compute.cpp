#include "compute.hpp"

Compute::Compute(const Geometry *geom, const Parameter *param, const Communicator *comm):
    _geom (geom),
    _param (param),
    _comm (comm)
{
    multi_real_t offset;
	offset[0] = _geom->Mesh()[0]; offset[1] = 0.5*_geom->Mesh()[1];
	_u   = new Grid(_geom, offset);
	_F   = new Grid(_geom, offset);
	offset[0] = 0.5*_geom->Mesh()[0]; offset[1] = _geom->Mesh()[1];
	_v   = new Grid(_geom, offset);
	_G   = new Grid(_geom, offset);
	offset[0] = 0.5*_geom->Mesh()[0]; offset[1] = 0.5*_geom->Mesh()[1];
	_p   = new Grid(_geom, offset);
	_rhs = new Grid(_geom, offset);
	_tmp = new Grid(_geom, offset);


    _t = 0.;

    _epslimit = _param->Eps();
    //std::cout << "vor Solver" << std::endl;
    
    //_solver =  new SOR(_geom, _param->Omega());
    _solver = new RedOrBlackSOR(_geom, _param->Omega(), _comm);
    
    _dtlimit = _param->Dt();
    // Init time
    _t = 0.0;
    _geom->Update_U(_u);
    _geom->Update_V(_v);
    _geom->Update_P(_p);
}

Compute::~Compute(){
    delete _p;
    delete _u;
    delete _v;
    delete _F;
    delete _G;
    delete _rhs;
    delete _tmp;
    delete _solver;
}
 /// Execute one time step of the fluid simulation (with or without debug info)
  // @ param printInfo print information about current solver state (residual
  // etc.)
void Compute::TimeStep(bool printInfo){
    //Der Algorithmus wie er auf S. 22 im Skript steht:
    //1) compute dt (genaue Berechnung S.22)

    //Gets dt from input file
    real_t dt = 10000;
    if ( _param->Dt() != 0.0)
     dt = _param->Dt();

    const multi_real_t &h = _geom->Mesh();

    // Check diffusiove operator limitation (S.25)
    _dtlimit = _param->Tau() * (_param->Re()/2) * ((h[0]*h[0]*h[1]*h[1])/(h[0]*h[0]+h[1]*h[1]));
    if(_dtlimit < dt) {
    dt = _dtlimit;
    if(printInfo){ 
        std::cout << "Thread "<< _comm->ThreadNum() << " : Time Step Diffusive Limitation dt = " << dt << std::endl;
    }
  }
  // Convection Operator limitation (S25)
    _dtlimit = _param->Tau() * std::min(h[0] / _u->AbsMax(), h[1] / _v->AbsMax());
    if(_dtlimit < dt) {
    dt = _dtlimit;
    if(printInfo){
        std::cout << "Thread "<< _comm->ThreadNum() << " : Time Step Linitation Convection Operator dt = " << dt << " umax Abs = " << _u->AbsMax()<< std::endl;
    }
  }
    dt = _comm->geatherMin(dt);
    if(_comm->ThreadNum() == 0){
        std::cout << "Current Time " << _t << ", Timestep " << dt << std::endl;
    }
    

    //2) boundary_val

    _geom->Update_U(_u);
    _geom->Update_V(_v);
    _geom->Update_P(_p);
    _geom->Update_V(_G);
    _geom->Update_U(_F);
    /// Since we have now more Grids, to fulfill the required inequalities for stability
    ///we need to take the minimum of all the grids.
    
    MPI_Barrier(MPI_COMM_WORLD);
    //3) compute _F and _G (vorläufige Geschwindigkeiten)

    ///After the computation of the MomentumEquations, we need to communicate the velocities
    MomentumEqu(dt);
    _comm->copyBoundary(_F);
    _comm->copyBoundary(_G);

    //4) compute _rhs

    RHS(dt);

    //5) solve Poisson equation with SOR solver

    real_t res = 0;
    for (index_t i = 1; i<=_param->IterMax(); i++){
        res = _solver->Cycle(_p, _rhs);
        ///everybody needs the same residual afterwards
        res = _comm->geatherMax(res);
        _geom->Update_P(_p);
         //if(printInfo) {std::cout <<" Time " << _t << " Interation : " << i << " residual = " << res <<  std::endl;}
        if (res < _epslimit)
        {
        //if(printInfo) {std::cout <<"Convergence after " << i << " iterations" <<  std::endl;}
        if(printInfo){
            std::cout <<"Thread " << _comm->ThreadNum() << " : Convergence after " << i << " iterations" <<  std::endl;
        }
            break;
        }

    }

    //6) compute _u und _v
    NewVelocities(dt);

        _comm->copyBoundary(_u);
        _comm->copyBoundary(_v);

    _t+=dt;

}

/// Compute the new velocites u,v
void Compute::NewVelocities(const real_t &dt){
    for(InteriorIterator it = InteriorIterator(_geom); it.Valid(); it.Next()){
        _u->Cell(it) = _F->Cell(it) - dt * _p->dx_r(it);
        _v->Cell(it) = _G->Cell(it) - dt * _p->dy_r(it);
    }
    _geom->Update_U(_u);
    _geom->Update_V(_v);
}

/// Compute the temporary velocites F,G
void Compute::MomentumEqu(const real_t &dt){
    //externe Kraft fehlt noch
    for(InteriorIterator it = InteriorIterator(_geom); it.Valid(); it.Next()){
        _F->Cell(it) = _u->Cell(it) +
        dt*(((_u->dxx(it) + _u->dyy(it))/_param->Re())
        -_u->DC_udu_x(it,_param->Alpha())
        -_u->DC_vdu_y(it,_param->Alpha(),_v));

        _G->Cell(it) = _v->Cell(it)
        + dt*(((_v->dxx(it) + _v->dyy(it))/_param->Re())
        -_v->DC_vdv_y(it,_param->Alpha())
        -_v->DC_udv_x(it,_param->Alpha(),_u));
    }
    _geom->Update_V(_G);
    _geom->Update_U(_F);
}

/// Compute the RHS of the poisson equation
void Compute::RHS(const real_t &dt){
    for(InteriorIterator it = InteriorIterator(_geom); it.Valid(); it.Next()){
        _rhs->Cell(it) = 1.0/dt * (_F->dx_l(it)+_G->dy_l(it));
    }
}
/// Returns the simulated time in total
const real_t &Compute::GetTime() const{
    return _t;
}
/// Returns the pointer to U
const Grid* Compute::GetU() const
{return _u;}
/// Returns the pointer to V
const Grid* Compute::GetV() const
{return _v;}
/// Returns the pointer to P
const Grid* Compute::GetP() const
{return _p;}
/// Returns the pointer to RHS
const Grid* Compute::GetRHS() const
{return _rhs;}
/// Computes and returns the absolute velocity
const Grid *Compute::GetVelocity(){

  for(Iterator it(_geom); it.Valid(); it.Next()) {
    //Mittelung auf den Mittelpunkt der Zelle
    real_t uMean = (_u->Cell(it.Left()) + _u->Cell(it))/2;
    real_t vMean = (_v->Cell(it) + _v->Cell(it.Down()))/2;
    _tmp->Cell(it) = sqrt(uMean*uMean + vMean*vMean);
  }
  return _tmp;
}


/// Computes and returns the vorticity
const Grid *Compute::GetVorticity(){

    for(Iterator it(_geom); it.Valid(); it.Next()) {
		_tmp->Cell(it) = _u->dy_l(it) - _v->dx_l(it);
    }
    return _tmp;
}
const Grid* Compute::GetStream() {

	for (Iterator it(_geom);it.Valid();it.Next()) {
            /// first calculate the integral in the x- direction
            /// need to separate two cases: 1) if those are the bottom processes
            ///                             2) if those are not the bottom processes
        if (it < _geom->Size()[0]){
            if (_comm->isBottom()){
                if (it.Pos()[1] == 0 && it.Pos()[0] == 0)
                    _tmp->Cell(it) = 0;
                else
                    _tmp->Cell(it) = _tmp->Cell(it.Left()) + _geom->Mesh()[0] * _v->Cell(it);
            }
            ///if it not bottom, yet another non- bottom processes, we can fill the first x-line with zeros, as it will be
            ///communicated over anyway
            else {
                _tmp->Cell(it) = 0;
            }
        }
        else if (it==_geom->Size()[0]){
                ///communicate x
                 _comm->CommunicateStreamLineX(_tmp);
        }
        else {
            _tmp->Cell(it) = _tmp->Cell(it.Down()) + _u->Cell(it)*_geom->Mesh()[1];
        }
    }
    ///communicate y
    _comm->CommunicateStreamLineY(_tmp);

	return _tmp;
}
