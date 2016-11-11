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

void Compute::TimeStep(bool printInfo){
    //Der Algorithmus wie er auf S. 22 im Skript steht:
    //1) compute dt (genaue Berechnung S.22)//TODO:hab nicht gecheckt wie wir den Zeitschritt berechnen sollen
    if(printInfo) std::cout << "Set timestep" << std::endl;
    real_t dt = 0.1; //vorläufig
    //2) boundary_val
    if(printInfo) std::cout << "Setting boundary values" << std::endl;
    //3) compute _F and _G (vorläufige Geschwindigkeiten) //TODO:externe Kraft fehlt noch
    if(printInfo) std::cout << "Compute F and G" << std::endl;
    for(InteriorIterator it = InteriorIterator(_geom); it.Valid(); it.Next()){
        _F->Cell(it) = _u->Cell(it) + dt*(1/(_param->Re())*(_u->dxx(it) + _u->dyy(it)) - 2*_u->DC_udu_x(it,_param->Alpha())-_u->DC_vdu_y(it,_param->Alpha(),_v)-_v->DC_vdu_y(it,_param->Alpha(),_u));
        _G->Cell(it) = _v->Cell(it) + dt*(1/(_param->Re())*(_v->dxx(it) + _v->dyy(it)) - 2*_v->DC_vdv_y(it,_param->Alpha())-_u->DC_udv_x(it,_param->Alpha(),_v)-_u->DC_udv_x(it,_param->Alpha(),_v));
    }
    //4) compute _rhs
    if(printInfo) std::cout << "Compute rhs" << std::endl;
    for(InteriorIterator it = InteriorIterator(_geom); it.Valid(); it.Next()){
        _rhs->Cell(it) = 1/dt * (_F->dx_l(it)+_G->dy_l(it));
    }
    //5) solve Poisson equation with SOR solver
    if(printInfo) std::cout << "Starting SOR solver" << std::endl;
    //6) compute _u und _v
    if(printInfo) std::cout << "Compute u and v" << std::endl;
    //7) output  _u _v und _p
    if(printInfo) std::cout << "Output u, v and p" << std::endl;
}

const Grid* Compute::GetU() const
{return _u;}

const Grid* Compute::GetV() const
{return _v;}

const Grid* Compute::GetP() const
{return _p;}

const Grid* Compute::GetRHS() const
{return _rhs;}
