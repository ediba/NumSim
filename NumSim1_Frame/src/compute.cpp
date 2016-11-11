#include "compute.hpp"
#include "geometry.hpp"
#include "solver.hpp"
#include "iterator.hpp"
#include "grid.hpp"
#include "parameter.hpp"

#include <math.h>
#include <stdio.h>

Compute::Compute(const Geometry *geom, const Parameter *param){
    _geom = geom;
    _param = param;
    _p = new Grid(geom,{_geom->Mesh()[0]*0.5,_geom->Mesh()[1]*0.5});
    _u = new Grid(geom,{_geom->Mesh()[0],_geom->Mesh()[1]*0.5});
    _v = new Grid(geom,{_geom->Mesh()[0]*0.5,_geom->Mesh()[1]});
    _F = new Grid(geom);
    _G = new Grid(geom);
    _rhs = new Grid(geom);
    _tmp = new Grid(geom);
    _t = 0.;
    //_dtlimit
    _epslimit = _param->Eps();
    //_solver = new SOR(geom,_param->Omega()); //TODO:somehow doesnt work
    
}

Compute::~Compute(){
    delete _p;
    delete _u;
    delete _v;
    delete _F;
    delete _G;
    delete _rhs;
    delete _tmp;
    //delete _solver; //TODO:uncomment after issue above is solved
}

const Grid* Compute::GetU() const
{return _u;}

const Grid* Compute::GetV() const
{return _v;}

const Grid* Compute::GetP() const
{return _p;}

const Grid* Compute::GetRHS() const
{return _rhs;}
