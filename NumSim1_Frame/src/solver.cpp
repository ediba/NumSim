#include "solver.hpp"
#include "iterator.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include <cmath>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;
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

//real_t SOR:: Cycle(Grid *grid, const Grid *rhs) const{
real_t SOR:: Cycle(Grid *grid, const Grid *rhs){
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
RedOrBlackSOR::RedOrBlackSOR (const Geometry* geom, const real_t& omega, const Communicator* comm) : SOR(geom,omega), _comm(comm) {
    std::cout<<"RedOrBlackSOR constructed with omeaga = " <<_omega<<std::endl;
}
// Destructor
RedOrBlackSOR::~RedOrBlackSOR(){}

//real_t RedOrBlackSOR::Cycle(Grid *grid, const Grid *rhs) const{
real_t RedOrBlackSOR::Cycle(Grid *grid, const Grid *rhs){
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

//real_t RedOrBlackSOR::RedCycle (Grid* grid, const Grid* rhs) const{
real_t RedOrBlackSOR::RedCycle (Grid* grid, const Grid* rhs){
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
//real_t RedOrBlackSOR::BlackCycle (Grid* grid, const Grid* rhs) const{
real_t RedOrBlackSOR::BlackCycle (Grid* grid, const Grid* rhs) {
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
    _N = -1;
    _maxN = numOfRef;
    for(index_t i=0; i<=numOfRef; i++){
        _geometries.push_back(new Geometry(*_geom, i));
        _error.push_back(new Grid(_geometries[i]));
        _res.push_back(new Grid(_geometries[i]));
        _solver.push_back(new RedOrBlackSOR(_geometries[i], (real_t)1., comm));
        //_solver.push_back(new SOR(_geometries[i], (real_t)1.));
    }
}

Grid* Multigrid::returnResiduum ( index_t level){
    return _res[level];
}
Grid* Multigrid::returnError (index_t level){
    return _error[level];
}
//Restric Funktion Fine to Coarse
void Multigrid::GetResiduals(Grid* p, const Grid* rhs, Grid* res, index_t ref)const{
    for(InteriorIterator it(_geometries[ref]); it.Valid(); it.Next() ){
        res->Cell(it) = localRes(it, p, rhs);
        //std::cout << "res of cell "<<it << " = " << res->Cell(it) << std::endl;
    }
}
 void Multigrid::restrict(Grid* pFine, Grid* const pCoarse,  const Grid* rhsFine, Grid* const  rhsCoarse, index_t ref) const{
     pCoarse->Initialize(0);
     rhsCoarse->Initialize(0);
     
      for(InteriorIterator it(_geometries[ref+1]); it.Valid(); it.Next()) {
        multi_index_t value2= it.Pos();
        value2[0] = value2[0]*2-1;
        value2[1] = value2[1]*2-1;
        
        const index_t value = pFine->IterFromPos(value2);
        const Iterator it2(_geometries[ref],value);
        
            rhsCoarse->Cell(it) = -0.25* ( localRes(it2,pFine, rhsFine)+ localRes(it2.Right(),pFine, rhsFine)+ localRes(it2.Top(), pFine, rhsFine) + localRes(it2.Top().Right(),pFine, rhsFine) );
    }
}

 void Multigrid::interCorse2Fine(Grid* pFine, Grid* pCoarse, index_t ref) const{

    for(InteriorIterator it(_geometries[ref+1]); it.Valid(); it.Next()) {
        multi_index_t value2= it.Pos();
        value2[0] = value2[0]*2-1;
        value2[1] = value2[1]*2-1;
        const index_t value = pFine->IterFromPos(value2);
        const Iterator it2(_geometries[ref],value);
        //0 oder -1
            pFine->Cell(it2)            += pCoarse->Cell(it);                  //TODO:Vorzeichen
            pFine->Cell(it2.Right())    += pCoarse->Cell(it);
            pFine->Cell(it2.Top())      += pCoarse->Cell(it);
            pFine->Cell(it2.Top().Right()) += pCoarse->Cell(it);

    }
}
void Multigrid::Boundaries(Grid* pFine, Grid* rhsCoarse, index_t ref) const{
    BoundaryIterator bit(_geometries[ref+1]);

        if (_comm->isBottom()){
            bit.SetBoundary(1);
            bit.Next();
            
            while(bit.Valid()){
            multi_index_t value2 = bit.Pos();
                
            if(value2[0]*2-1<0)value2[0]=0;
            else value2[0] = value2[0]*2-1;
            
            
            value2[1] = 0;
            if(value2[0]<0) value2[0] = 0;
            else if( value2[0] >_geometries[ref]->Size()[0]+1) value2[0] = _geometries[ref]->Size()[0]+1;
            if(value2[1]<0) value2[1] = 0;
            else if( value2[1] >_geometries[ref]->Size()[1]+1) value2[1] = _geometries[ref]->Size()[1];
            
            const index_t value = pFine->IterFromPos(value2);
            const Iterator bit2(_geometries[ref],value);
            
            rhsCoarse->Cell(bit)= -0.5*(pFine->dy_r(bit2)+pFine->dy_r(bit2.Right()));
            bit.Next();
            }
        }

        if (_comm->isRight()){
            bit.SetBoundary(2);
            bit.Next();
            
            while(bit.Valid()){
                
            multi_index_t value2 = bit.Pos();
            value2[0] = value2[0]*2-1;
            value2[1] = value2[1]*2-1;
            
            if(value2[0]<0) value2[0] = 0;
            else if( value2[0] >_geometries[ref]->Size()[0]+1) value2[0] = _geometries[ref]->Size()[0]+1;
            if(value2[1]<0) value2[1] = 0;
            else if( value2[1] >_geometries[ref]->Size()[1]+1) value2[1] = _geometries[ref]->Size()[1];
                
            const index_t value = pFine->IterFromPos(value2);
            const Iterator bit2(_geometries[ref],value);
        
            rhsCoarse->Cell(bit)= 0.5*(pFine->dx_l(bit2)+pFine->dx_l(bit2.Top()));
            bit.Next();
            }
        }

        if (_comm->isTop()){
            bit.SetBoundary(3);
            bit.Next();
            
            while(bit.Valid()){
                
            multi_index_t value2 = bit.Pos();
            value2[0] = value2[0]*2-1;
            value2[1] = value2[1]*2-1;
            
            if(value2[0]<0) value2[0] = 0;
            else if( value2[0] >_geometries[ref]->Size()[0]+1) value2[0] = _geometries[ref]->Size()[0]+1;
            if(value2[1]<0) value2[1] = 0;
            else if( value2[1] >_geometries[ref]->Size()[1]+1) value2[1] = _geometries[ref]->Size()[1];
                
            const index_t value = pFine->IterFromPos(value2);
            const Iterator bit2(_geometries[ref],value);
            
            //_res[ref+1]->Cell(bit)= 0.5*(_p[ref]->dy_l(bit2)+_p[ref]->dy_l(bit2.Left()));
            rhsCoarse->Cell(bit)= 0.5*(pFine->dy_l(bit2)+pFine->dy_l(bit2.Right()));
            bit.Next();
            }
        }
        
        if (_comm->isLeft()){
            bit.SetBoundary(4);
            
            bit.Next();
            
            while(bit.Valid()){
                
            multi_index_t value2 = bit.Pos();
            value2[0] = 0;
            if(value2[1]*2-1<0)value2[1]=0;
            else value2[1] = value2[1]*2-1;
            
            if(value2[0]<0) value2[0] = 0;
            else if( value2[0] >_geometries[ref]->Size()[0]+1) value2[0] = _geometries[ref]->Size()[0]+1;
            if(value2[1]<0) value2[1] = 0;
            else if( value2[1] >_geometries[ref]->Size()[1]+1) value2[1] = _geometries[ref]->Size()[1];
            //_res[ref+1]->Cell(bit)= 0.5*(_p[ref]->dx_r(bit2)+_p[ref]->dx_r(bit2.Bottom()));
            
            const index_t value = pFine->IterFromPos(value2);
            const Iterator bit2(_geometries[ref],value);
            rhsCoarse->Cell(bit)= 0.5*(pFine->dx_r(bit2)+pFine->dx_r(bit2.Top()));
            bit.Next();
            }
        }
}

real_t Multigrid::smooth(Grid *grid, const Grid *rhs) {
    real_t res = 0;
    if(_N == 0){
        _geometries[0]->Update_P(grid);
        res = _solver[0]->Cycle(grid, rhs);
        
        _geometries[0]->Update_P(grid);
        
        GetResiduals(grid, rhs, _res[_N], _N);
        // res = _solver[0]->Cycle(grid, rhs);
         //_geometries[0]->Update_P(grid);
         //res = _solver[0]->Cycle(grid, rhs);
    }
    else{
         //_geometries[_N]->BoundaryUpdateCoarse(grid, rhs);
         _geometries[_N]->Update_P(grid);
         res = _solver[_N]->Cycle(grid, rhs);
         _geometries[_N]->Update_P(grid);
        //_geometries[_N]->BoundaryUpdateCoarse(grid, rhs);
        //res = _solver[_N]->Cycle(grid, rhs);
        //_geometries[_N]->BoundaryUpdateCoarse(grid, rhs);
        //res = _solver[_N]->Cycle(grid, rhs);
    }
    return res;
}
void Multigrid::GetResultsPerLevel(int level){

    ofstream file;

    ostringstream fileWrite;
    fileWrite << "errorPlotLevel_"<<level << ".txt";
    std::string fileName = fileWrite.str();

    file.open(fileName.c_str());

     for(InteriorIterator it(_geometries[level]); it.Valid(); it.Next()) {
        file<<_error[level]->Cell(it)<<" "<<endl;
     }

    //fileWrite << "Writing this to a file.\n";
    file.close();

    ofstream residuumFile;

    ostringstream residuumFileWrite;
    residuumFileWrite << "residuumPlotLevel_"<<level << ".txt";
    std::string residuumFileName = residuumFileWrite.str();

    residuumFile.open(residuumFileName.c_str());

     for(InteriorIterator it(_geometries[level]); it.Valid(); it.Next()) {
        residuumFile<<_res[level]->Cell(it)<<" "<<std::endl;
     }

    //fileWrite << "Writing this to a file.\n";
    residuumFile.close();

}

Multigrid::~Multigrid(){}

real_t Multigrid::Cycle(Grid *grid, const Grid *rhs) {
    _N++;
    real_t res = 0;
    res = smooth(grid,rhs);
    //Abbruch bedingung
    if(_N==_maxN) {_N--; return res;}
    
    restrict(grid, _error[_N+1], rhs, _res[_N+1], _N);
    Boundaries(grid, _res[_N+1], _N);
    
    //If results need to be plotted
    //GetResultsPerLevel(_N);
    
    Cycle(_error[_N+1], _res[_N+1]);
    
    interCorse2Fine(grid, _error[_N+1],_N);
    res = smooth(grid,rhs);
    _N--;
    
    return res;
    /*
     //TODO Alter nicht rekursiver Algorithmus (todos sind nur für übersichtlichkeit
    real_t res = 0;
    _geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    _geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    _geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    
    // //TODO Stufe 0 nach 1
    //Resiuduals and restrict
    restrict(grid, _error[1], rhs, _res[1], (index_t)0);
    Boundaries(grid, _res[1], (index_t) 0);
    
    _geometries[1]->BoundaryUpdateCoarse(_error[1], _res[1]);
    res = _solver[1]->Cycle(_error[1], _res[1]);
    _geometries[1]->BoundaryUpdateCoarse(_error[1], _res[1]);
    res = _solver[1]->Cycle(_error[1], _res[1]);
    _geometries[1]->BoundaryUpdateCoarse(_error[1], _res[1]);
    res = _solver[1]->Cycle(_error[1], _res[1]);

    //TODO Stufe 1 nach 2
    restrict(_error[1], _error[2], _res[1],  _res[2], (index_t)1);
    Boundaries(_error[1], _res[2], (index_t) 1);

    
    _geometries[2]->BoundaryUpdateCoarse(_error[2], _res[2]);
    res = _solver[2]->Cycle(_error[2], _res[2]);
    _geometries[2]->BoundaryUpdateCoarse(_error[2], _res[2]);
    res = _solver[2]->Cycle(_error[2], _res[2]);
    
    //TODO Stufe 2 nach 1
    interCorse2Fine(_error[1], _error[2], (index_t) 1);
    
    _geometries[1]->BoundaryUpdateCoarse(_error[1], _res[2]);
    res = _solver[1]->Cycle(_error[1], _res[1]);
    _geometries[1]->BoundaryUpdateCoarse(_error[1], _res[2]);
    res = _solver[1]->Cycle(_error[1], _res[1]);
    
    //TODO Stufe 1 nach 0
    interCorse2Fine(grid, _error[1],(index_t) 0);
    
    
    //Glätten
    _geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    _geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);
    _geometries[0]->Update_P(grid);
    res = _solver[0]->Cycle(grid, rhs);

    return res;*/
}