#include "compute.hpp"
#include "geometry.hpp"
#include "solver.hpp"
#include "iterator.hpp"
#include "grid.hpp"
#include "parameter.hpp"

#include <math.h>
#include <stdio.h>


const Grid* Compute::GetU() const
{return _u;}

const Grid* Compute::GetV() const
{return _v;}

const Grid* Compute::GetP() const
{return _p;}

const Grid* Compute::GetRHS() const
{return _rhs;}
