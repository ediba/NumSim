#include "solver.hpp"
#include "iterator.hpp"
#include "geometry.hpp"
#include "grid.hpp"
#include <cmath>

///Implementing the solver class
Solver::Solver(const Geometry *geom){
  _geom = geom;
}

Solver::~Solver() {

}

/// Returns the residual at [it] for the pressure-Poisson equation
real_t Solver::localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const {
    return ((grid->Cell(it.Left())+grid->Cell(it.Right()))/(_geom->Mesh()[0])/(_geom->Mesh()[0])+(grid->Cell(it.Top())+grid->Cell(it.Down()))/(_geom->Mesh()[1])/(_geom->Mesh()[1]))-rhs->Cell(it);
}


///Implementing the actual instance of a solver class (SOR)

SOR::SOR(const Geometry *geom, const real_t &omega): Solver(geom) {
  _omega = omega;}

SOR::~SOR(){

}

real_t SOR:: Cycle(Grid *grid, const Grid *rhs) const{
    const real_t h1 = _geom->Mesh()[0];
    const real_t h2 = _geom->Mesh()[1];
    //the multiplier is not changing for any point in grid, that is why it is before the loop
    const real_t multiplier = _omega*0.5*h1*h1*h2*h2/(h1*h1+h2*h2);
    InteriorIterator iter(_geom);
    iter.First();
    real_t totalRes(0.0);
    
    while (iter.Valid()){
        real_t residual = localRes(iter, grid, rhs);
        totalRes += residual*residual;
        grid->Cell(iter) =(1.0-_omega)*grid->Cell(iter) + multiplier * residual;//old: -=multiplier*residual;
        iter.Next();
    }
    //for averaging
    return sqrt((totalRes)/(_geom->Size()[0]*_geom->Size()[1]));
}

