/*
 * Copyright (C) 2015   Malte Brunn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
//------------------------------------------------------------------------------
#include "typedef.hpp"
#include "comm.hpp"
#include <vector>
//------------------------------------------------------------------------------
#ifndef __SOLVER_HPP
#define __SOLVER_HPP
//------------------------------------------------------------------------------

/** abstract base class for an iterative solver
*/
class Solver {
public:
  /// Constructor of the abstract Solver class
  Solver(const Geometry *geom);
  /// Destructor of the Solver Class
  virtual ~Solver();

  /// This function must be implemented in a child class
  // @param [in][out] grid current values
  // @param [in]      rhs  right hand side values
  // @returns accumulated residual
  //virtual real_t Cycle(Grid *grid, const Grid *rhs) const = 0;
  virtual real_t Cycle(Grid *grid, const Grid *rhs) = 0;

protected:
  const Geometry *_geom;

  /// Returns the residual at [it] for the pressure-Poisson equation
  real_t localRes(const Iterator &it, const Grid *grid, const Grid *rhs) const;
};

//------------------------------------------------------------------------------

/** concrete SOR solver
*/
class SOR : public Solver {
public:
  /// Constructs an actual SOR solver
  SOR(const Geometry *geom, const real_t &omega);
  /// Destructor
  ~SOR();

  /// Returns the total residual and executes a solver cycle
  // @param grid current pressure values
  // @param rhs right hand side
  //virtual real_t Cycle(Grid *grid, const Grid *rhs) const;
  virtual real_t Cycle(Grid *grid, const Grid *rhs);

protected:
  real_t _omega;
};

//------------------------------------------------------------------------------

/** concrete Red or Balck SOR solver
 */
class RedOrBlackSOR : public SOR {
    private:
     const Communicator* _comm;
public:
	/// Constructs an actual SOR solver
	RedOrBlackSOR (const Geometry* geom, const real_t& omega,  const Communicator* comm);
	/// Destructor
	~RedOrBlackSOR();

//         real_t Cycle(Grid *grid, const Grid *rhs) const;
// 	real_t RedCycle (Grid* grid, const Grid* rhs) const;
// 	real_t BlackCycle (Grid* grid, const Grid* rhs) const;
        
        real_t Cycle(Grid *grid, const Grid *rhs);
	real_t RedCycle (Grid* grid, const Grid* rhs);
	real_t BlackCycle (Grid* grid, const Grid* rhs);
};


class Multigrid : public Solver {
public:
    Multigrid (const Geometry *geom, const Communicator *comm, index_t numOfRef);
    ~Multigrid();
    real_t Cycle(Grid *grid, const Grid *rhs);
    
    //nur für TestZecke Public
    void restrict(Grid* pFine, Grid* const pCoarse, const Grid* rhsFine, Grid* const  rhsCoarse, index_t ref) const;
    void interCorse2Fine(Grid* pFine, Grid* pCoarse, index_t ref) const;
    void TestFunktion(Grid* grid);
    void Boundaries(Grid* pFine, Grid* rhsCoarse, index_t ref) const;
    Grid*  returnResiduum ( index_t level);
    Grid* returnError (index_t level);
    void GetResiduals(Grid* p, const Grid* rhs, Grid* res, index_t ref)const;
    real_t smooth(Grid *grid, const Grid *rhs);
    void GetResultsPerLevel(int level);
    std::vector<Geometry*> _geometries;
    std::vector<Grid*> _res;
    std::vector<Grid*> _error;
    
protected:
    
private:
    index_t _maxN;
    int _N;
    
//     std::vector<Geometry*> _geometries;
//     std::vector<Grid*> _res;
//     std::vector<Grid*> _error;
    std::vector<RedOrBlackSOR*> _solver;
    //std::vector<SOR*> _solver;
    const Communicator* _comm;
    //void restrict(Grid* pFine, Grid* pCoarse, Grid* rhsFine, Grid* rhsCoarse, index_t ref);
    //void interCorse2Fine(Grid* pFine, Grid* pCoarse, index_t ref);

};

class CGSolver : public Solver {
public:
    CGSolver (const Geometry *geom, const Communicator *comm);
    ~CGSolver();
    real_t Cycle(Grid *grid, const Grid *rhs);
    
    void GetResiduals(Grid* p, const Grid* rhs, Grid* res);
    void LaPlace(Grid* grid, Grid* result);
    real_t scalarProduct(Grid* first, Grid* second);
    void add(Grid* first, Grid* second, Grid* result, real_t multiplier = 1.);
    void copy(Grid* origin, Grid* result);
    real_t sumResiduals(Grid* grid);
private:
    real_t _alpha;
    Grid* _d;
    Grid* _z;
    //Grid* _r;
    const Communicator* _comm;
};
//------------------------------------------------------------------------------
#endif // __SOLVER_HPP
