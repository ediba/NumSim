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
        _grids.push_back(new Grid(_geometries[i]));
    }
}

Multigrid::~Multigrid(){}

real_t Multigrid::Cycle(Grid *grid, const Grid *rhs) const {}