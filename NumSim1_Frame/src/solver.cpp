#include "solver.hpp"
#include "iterator.hpp"
#include "geometry.hpp"
#include "grid.hpp"

///Implementing the solver class
Solver::Solver(const Geometry *geom){
  _geom = geom;
}

Solver::~Solver() {

}


///Implementing the actual instance of a solver class (SOR)

SOR::SOR(const Geometry *geom, const real_t &omega)
  : Solver(geom) {
  _omega = omega;
}

SOR::~SOR(){

}
