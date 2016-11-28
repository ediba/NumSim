#include "grid.hpp"
#include "iterator.hpp"
#include "geometry.hpp"
#include "parameter.hpp"
#include "solver.hpp"
#include "compute.hpp"
#include <cmath>
#include <algorithm>
#include <cstdio>
#include "zeitgeist.hpp"
//------------------------------------------------------------------------------
Compute::Compute (const Geometry* geom, const Parameter* param) : _geom(geom), _param(param) {
	multi_real_t offset;
	offset[0] = 0.0; offset[1] = -0.5*_geom->Mesh()[1];
	_u   = new Grid(_geom, offset);
	_F   = new Grid(_geom, offset);
	offset[0] = -0.5*_geom->Mesh()[0]; offset[1] = 0.0;
	_v   = new Grid(_geom, offset);
	_G   = new Grid(_geom, offset);
	offset[0] = -0.5*_geom->Mesh()[0]; offset[1] = -0.5*_geom->Mesh()[1];
	_p   = new Grid(_geom, offset);
	_rhs = new Grid(_geom, offset);
	_tmp = new Grid(_geom, offset);

	_solver = new SOR(_geom, _param->Omega());

	_u->Initialize(0);
	_v->Initialize(0);
	_p->Initialize(0);

	_geom->Update_P(_p);
	_geom->Update_U(_u);
	_geom->Update_V(_v);

	_t = 0.0;
	_dtlimit = _param->Re()*_geom->Mesh()[0]*_geom->Mesh()[0]*_geom->Mesh()[1]*_geom->Mesh()[1]/(2*((_geom->Mesh()[0]*_geom->Mesh()[0])+(_geom->Mesh()[1]*_geom->Mesh()[1])));
	_epslimit =  _param->Eps()*_param->Eps()*(_geom->Size()[0])*(_geom->Size()[1]);
}
//------------------------------------------------------------------------------
Compute::~Compute () {
	delete _u;
	delete _v;
	delete _p;
	delete _F;
	delete _G;
	delete _rhs;
	delete _tmp;
	delete _solver;
}
//------------------------------------------------------------------------------
void Compute::TimeStep (bool debug) {
	ZeitGeist zg;
	zg.Start();
	if (debug) printf("%3.5lf", _t);

	real_t dt = _param->Dt();
	if (dt == 0.0) {
		dt = std::min<real_t>(_geom->Mesh()[0]/_u->AbsMax(),_geom->Mesh()[1]/_v->AbsMax());
		dt = std::min<real_t>(dt,_dtlimit);
		dt *= _param->Tau();
	}
	if (debug) printf(" %le%5lu",dt,zg.Step());

	MomentumEqu(dt);
	_geom->Update_U(_F);
	_geom->Update_V(_G);
	if (debug) printf("%10lu",zg.Step());

	RHS(dt);
	_geom->Update_P(_rhs);
    index_t iter = _param->IterMax();
    real_t residual;
    if (debug) printf("%5lu",zg.Step());
    do {
		residual  = _solver->Cycle(_p,_rhs);
		_geom->Update_P(_p);
    } while (--iter > 0 && residual > _epslimit);
	if (debug) printf("%10lu (%5u, %le)",zg.Step(),_param->IterMax()-iter,residual);

	NewVelocities(dt);
	_geom->Update_U(_u);
	_geom->Update_V(_v);
	if (debug) printf("%5lu\n",zg.Step());
	_t += dt;
}
//------------------------------------------------------------------------------
const real_t& Compute::GetTime () const { return _t; }
//------------------------------------------------------------------------------
const Grid* Compute::GetU () const { return _u; }
//------------------------------------------------------------------------------
const Grid* Compute::GetV () const { return _v; }
//------------------------------------------------------------------------------
const Grid* Compute::GetP () const { return _p; }
//------------------------------------------------------------------------------
const Grid* Compute::GetRHS () const { return _rhs; }
//------------------------------------------------------------------------------
const Grid* Compute::GetVelocity() {
	Iterator it(_geom);
	real_t mid;
	for (it.First();it.Valid();it.Next()) {
		mid = _u->Cell(it) + _u->Cell(it.Left());
		_tmp->Cell(it)  = mid*mid;
		mid = _v->Cell(it) + _v->Cell(it.Down());
		_tmp->Cell(it) += mid*mid;
		_tmp->Cell(it) = sqrt(_tmp->Cell(it));
	}
	return _tmp;
}
//------------------------------------------------------------------------------
const Grid* Compute::GetVorticity() {
	Iterator it(_geom);
	for (it.First();it.Valid();it.Next()) {
		_tmp->Cell(it) = _u->dy_l(it) - _v->dx_l(it);
	}
	return _tmp;
}
//------------------------------------------------------------------------------
const Grid* Compute::GetStream() {
	Iterator it(_geom);
	for (it.First();it.Valid();it.Next()) {
		if (it.Pos()[1] == 0)
			_tmp->Cell(it) = 0;
		else
			_tmp->Cell(it) = _tmp->Cell(it.Down()) + _u->Cell(it)*_geom->Mesh()[1];
	}
	return _tmp;
}
//------------------------------------------------------------------------------
void Compute::NewVelocities (const real_t& dt) {
	InteriorIterator it(_geom);
	for (it.First();it.Valid();it.Next()) {
		_u->Cell(it) = _F->Cell(it) - dt*_p->dx_r(it);
		_v->Cell(it) = _G->Cell(it) - dt*_p->dy_r(it);
	}
}
//------------------------------------------------------------------------------
void Compute::MomentumEqu (const real_t& dt) {
	InteriorIterator it(_geom);
	for (it.First();it.Valid();it.Next()) {
		_F->Cell(it) = _u->Cell(it) +
			dt*((_u->dxx(it) + _u->dyy(it))/_param->Re() -
			_u->DC_udu_x(it,_param->Alpha()) -
			_u->DC_vdu_y(it,_param->Alpha(),_v));
		_G->Cell(it) = _v->Cell(it) +
			dt*((_v->dxx(it) + _v->dyy(it))/_param->Re() -
			_v->DC_udv_x(it,_param->Alpha(),_u) -
			_v->DC_vdv_y(it,_param->Alpha()));
	}
}
//------------------------------------------------------------------------------
void Compute::RHS (const real_t& dt) {
	InteriorIterator it(_geom);
	for (it.First();it.Valid();it.Next()) {
		_rhs->Cell(it) = (_F->dx_l(it) + _G->dy_l(it))/dt;
	}
}
//------------------------------------------------------------------------------
