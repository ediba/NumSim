#include "solver.hpp"
#include "iterator.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include <cmath>

///Implementing the solver class
Solver::Solver(const Geometry *geom) : _geom(geom) {}

Solver::~Solver() {}

/// Returns the residual at [it] for the pressure-Poisson equation
real_t Solver::localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const {
    //std::cout << " grid dxx = " << grid->dxx(it) << " grid->dyy(it) = " << grid->dyy(it) << " rhs->Cell(it) = " << rhs->Cell(it) <<std::endl;
    return grid->dxx(it) + grid->dyy(it) - rhs->Cell(it);
    //return ((grid->Cell(it.Left())+grid->Cell(it.Right()))/(_geom->Mesh()[0])/(_geom->Mesh()[0])+(grid->Cell(it.Top())+grid->Cell(it.Down()))/(_geom->Mesh()[1])/(_geom->Mesh()[1]))-rhs->Cell(it);
}


///Implementing the actual instance of a solver class (SOR)

SOR::SOR(const Geometry *geom, const real_t &omega) : Solver(geom) {
  _omega = omega;
}

SOR::~SOR(){}

real_t SOR:: Cycle(Grid *grid, const Grid *rhs) const{
    const real_t h1 = _geom->Mesh()[0];
    const real_t h2 = _geom->Mesh()[1];
    //the multiplier is not changing for any point in grid, that is why it is before the loop
    const real_t multiplier = _omega*0.5*h1*h1*h2*h2/((h1*h1)+(h2*h2));
    InteriorIterator iter(_geom);
    iter.First();
    real_t totalRes = 0;

    while (iter.Valid()){
        // In case of free geometry skips a non fluid cell
//         if(_geom->FreeGeometry()){
//             if(_geom->Flag(iter) != ' '){
//                 iter.Next();
//                 numOst++;
//             }
//         }
        real_t residual = localRes(iter, grid, rhs);
        totalRes += residual*residual;
        //grid->Cell(iter) =(1.0-_omega)*grid->Cell(iter) + multiplier * residual;//old: -=multiplier*residual;
        grid->Cell(iter) += multiplier * residual;
        iter.Next();
    }
    //for averaging
    return sqrt((totalRes)/(_geom->Size()[0]*_geom->Size()[1]));
}

/// Constructs an actual SOR solver
RedOrBlackSOR::RedOrBlackSOR (const Geometry* geom, const real_t& omega, const Communicator* comm) : SOR(geom,omega), _comm(comm) {}
// Destructor
RedOrBlackSOR::~RedOrBlackSOR(){}

real_t RedOrBlackSOR::Cycle(Grid *grid, const Grid *rhs) const{
    real_t totalRes = 0;
    totalRes += RedCycle(grid,rhs);

    //TODO: And diese Stelle kommt die Kommunikation zwischen den parallelen Grids (über Geometry?!)
    /// Communicate the pressure values
    _comm->copyBoundary(grid);
    totalRes += BlackCycle(grid,rhs);
     /// Communicate the pressure values
    _comm->copyBoundary(grid);

    //for averaging
    index_t t = _geom->NumInteriorBounds();
    //std::cout << " NumInteriorBounds = " << t<< std::endl;
    return sqrt((totalRes)/((_geom->Size()[0]*_geom->Size()[1])-t ));
}


real_t RedOrBlackSOR::RedCycle (Grid* grid, const Grid* rhs) const{
    const real_t h1 = _geom->Mesh()[0];
    const real_t h2 = _geom->Mesh()[1];
    //the multiplier is not changing for any point in grid, that is why it is before the loop
    const real_t multiplier = _omega*0.5*h1*h1*h2*h2/((h1*h1)+(h2*h2));
    RedIterator iter(_geom);
    iter.First();
    real_t totalRes = 0;
    
    index_t k = 0;
    while (iter.Valid()){
        if(_geom->NumInteriorBounds()!= 0){
            if(_geom->Flag(iter) != ' '){
                iter.Next();
                k++;
                continue;
            }
        }
        real_t residual = localRes(iter, grid, rhs);
        //std::cout<<"residual of iter "<< iter <<" = " << residual << std::endl;
        totalRes += residual*residual;
        //grid->Cell(iter) =(1.0-_omega)*grid->Cell(iter) + multiplier * residual;//old: -=multiplier*residual;
        grid->Cell(iter) += multiplier * residual;
        iter.Next();
    }
    //std::cout << "Skipped in iterator Red : " << k << std::endl;
    return totalRes;
}

real_t RedOrBlackSOR::BlackCycle (Grid* grid, const Grid* rhs) const{
    const real_t h1 = _geom->Mesh()[0];
    const real_t h2 = _geom->Mesh()[1];
    //the multiplier is not changing for any point in grid, that is why it is before the loop
    const real_t multiplier = _omega*0.5*h1*h1*h2*h2/((h1*h1)+(h2*h2));
    BlackIterator iter(_geom);
    iter.First();
    real_t totalRes = 0;
    
    index_t k = 0;

    while (iter.Valid()){
        if(_geom->NumInteriorBounds()!= 0){
            if(_geom->Flag(iter) != ' '){
                iter.Next();
                k++;
                continue;
            }
        }
        
        real_t residual = localRes(iter, grid, rhs);
        //std::cout<<"residual of iter "<< iter <<" = " << residual << std::endl;
        totalRes += residual*residual;
        //grid->Cell(iter) =(1.0-_omega)*grid->Cell(iter) + multiplier * residual;//old: -=multiplier*residual;
        grid->Cell(iter) += multiplier * residual;
        iter.Next();
    }
    //std::cout << "Skipped in iterator Red : " << k << std::endl;
    return totalRes;
}


Multigrid::Multigrid (const Geometry *geom, const Communicator *comm, index_t numOfRef) : Solver(geom){
    _comm=comm;
    for(index_t i=0; i<numOfRef; i++){
        _geometries.push_back(new Geometry(*_geom, i));
        _error.push_back(new Grid(_geometries[i]));
        _res.push_back(new Grid(_geometries[i]));
        _solver.push_back(new RedOrBlackSOR(_geometries[i], (real_t)1., comm));
    }
}
//Restric Funktion Fine to Coarse
 void Multigrid::restrict(Grid* pFine, Grid* const pCoarse,  const Grid* rhsFine, Grid* const  rhsCoarse, index_t ref) const{
//   for(InteriorIterator it(_geometries[ref+1]); it.Valid(); it.Next()) {
//         multi_index_t value2= it.Pos();
//         index_t value=(2*value2(0)-1)+(2*value2(1)-1)*(_geometries[ref+1]->Size()+2);
//         _error[ref]->Cell(it) = 0;
//         const InteriorIterator it2(_geometries[ref],value);
//         
//         if(ref==-1){
//             _res[ref+1]->Cell(it) = 0.25* ( localRes(it2,_p, _rhs) + localRes	(it2.Right(), _p, _rhs)+ localRes(it2.Top(), _p, _rhs) + localRes	(it2.Top().Right(), _p, _rhs) );
//         }
//         else {
//             _res[ref+1]->Cell(it) = 0.25* ( localRes(it2,_error[ref], _res[ref])+ 	localRes(it2.Right(),_error[ref], _res[ref])+ localRes(it2.Top(), 	_error[ref], _res[ref]) + localRes(it2.Top().Right(),_error[ref], 	_res[ref]) );
//         }
//     }
      for(InteriorIterator it(_geometries[ref+1]); it.Valid(); it.Next()) {
        multi_index_t value2= it.Pos();
        value2[0] = value2[0]*2-1;
        value2[1] = value2[1]*2-1;
        //const index_t value=(2*value2[0]-1)+(2*value2[1]-1)*(_geometries[ref+1]->Size()[0]+2);
        const index_t value = pFine->IterFromPos(value2);
        pCoarse->Cell(it) = 0;
        const Iterator it2(_geometries[ref],value);
        
            rhsCoarse->Cell(it) = 0.25* ( localRes(it2,pFine, rhsFine)+ localRes(it2.Right(),pFine, rhsFine)+ localRes(it2.Top(), pFine, rhsFine) + localRes(it2.Top().Right(),pFine, rhsFine) );
            std::cout << "local Res of Cell " << it << " = " << rhsCoarse->Cell(it) << std::endl;
    }
}

 void Multigrid::interCorse2Fine(Grid* pFine, Grid* pCoarse, index_t ref) const{
	
//     for(Iterator it(_geometries[ref+1]); it.Valid(); it.Next()) {
//         multi_index_t value2= it.Pos();
//         index_t value=(2*value2(0)-1)+(2*value2(1)-1)*(_geometries[ref+1]->Size()+2);
//         const Iterator it2(_geometries[ref],value);
//         //0 oder -1
//         if(ref == -1){
//                 _p->Cell(it2) += _error[ref+1]->Cell(it);
//             _p->Cell(it2.Right()) += _error[ref+1]->Cell(it);
//             _p->Cell(it2.Top()) += _error[ref+1]->Cell(it);
//             _p->Cell(it2.Top().Right()) += _error[ref+1]->Cell(it);
// 
//         }
//         else {
//             _error[ref]->Cell(it2) += _error[ref+1]->Cell(it);
//             _error[ref]->Cell(it2.Right()) += _error[ref+1]->Cell(it);
//             _error[ref]->Cell(it2.Top()) += _error[ref+1]->Cell(it);
//             _error[ref]->Cell(it2.Top().Right()) += _error[ref+1]->Cell(it);
//         }
//     }
    for(InteriorIterator it(_geometries[ref+1]); it.Valid(); it.Next()) {
        multi_index_t value2= it.Pos();
        value2[0] = value2[0]*2-1;
        value2[1] = value2[1]*2-1;
        const index_t value = pFine->IterFromPos(value2);
        const Iterator it2(_geometries[ref],value);
        //0 oder -1
            pFine->Cell(it2) += pCoarse->Cell(it);
            pFine->Cell(it2.Right()) += pCoarse->Cell(it);
            pFine->Cell(it2.Top()) += pCoarse->Cell(it);
            pFine->Cell(it2.Top().Right()) += pCoarse->Cell(it);

    }
}

Multigrid::~Multigrid(){}
// real_t Multigrid::Cycle(Grid *grid, const Grid *rhs) const{
//     grid->PrintGrid();
//     TestFunktion(grid);
// }
// void Multigrid::TestFunktion(Grid* grid){
//    // _error[0]->PrintGrid();
// }
real_t Multigrid::Cycle(Grid *grid, const Grid *rhs) const{
    real_t res = 0;
    //_error[0]->Initialize(0.);
    //_res[0]->Initialize(0.);
    //Glätten
    
    std::cout<<"p start" << std::endl;
    grid->PrintGrid();
    std::cout<<"rhs start" << std::endl;
    rhs->PrintGrid();
    
    //_geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    
    std::cout<<"p Nach Glätter"<<std::endl;
    grid->PrintGrid();
    
    //Resiuduals and restrict
    restrict(grid, _error[1], rhs, _res[1], (index_t)0);
    
    std::cout<<"Error 1.Stufe vor glätter"<<std::endl;
    _error[1]->PrintGrid();
    
    //_geometries[1]->Update_P(_error[1]);
    res = _solver[1]->Cycle(_error[1], _res[1]);

    std::cout<<"Error 1.Stufe nach glätter"<<std::endl;
    _error[1]->PrintGrid();
    
    interCorse2Fine(grid, _error[1],(index_t) 0);
    
    std::cout<<"p Vor 2. Glätter"<<std::endl;
    grid->PrintGrid();
    
    //Glätten
    //_geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    
    std::cout<<"p Nach 2. Glätter"<<std::endl;
    grid->PrintGrid();
    //std::cout<<"rhs Nach 2. Glätter"<<std::endl;
    //rhs->PrintGrid();
    
    
    
    return res;
}