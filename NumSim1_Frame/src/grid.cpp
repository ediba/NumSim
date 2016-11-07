#include "grid.hpp"
#include "geometry.hpp"
#include "iterator.hpp"
#include <cmath>        // std::abs, pow
#include <iostream>



Grid::Grid(const Geometry *geom, const multi_real_t &offset)
 {
    _geom(geom);
    _offset(offset);
    ///plus 2 ???
    _data = new real_t[_geom->Size()[0] * _geom->Size()[1]];
  this->Initialize(real_t(0.0));
}
Grid::Grid(const Geometry* geom)
{ Grid(geom, multi_real_t(0.0,0.0));}
Grid::~Grid()
{delete[] _data;}

void Grid::Initialize(const real_t& value)
{
    for(int i=0; i<_geom->Size()[0]*_geom->Size()[1]; i++)
    {_data[i] = value;}
}

real_t &Grid::Cell(const Iterator &it){
  return _data[it.Value()];
}


const real_t &Grid::Cell(const Iterator &it) const{
  return _data[it.Value()];
}



real_t Grid::dx_l(const Iterator& it) const
{return (_data[it] - _data[it.Left()])/((_geom->Mesh())[0]);}

real_t Grid::dx_r(const Iterator& it) const
{return (_data[it.Right()] - _data[it])/((_geom->Mesh())[0]);}

real_t Grid::dy_l(const Iterator& it) const
{return (_data[it] - _data[it.Down()])/((_geom->Mesh())[1]);}

real_t Grid::dy_r(const Iterator& it) const
{return (_data[it.Top()] - _data[it])/((_geom->Mesh())[1]);}
///


real_t Grid:: dxx(const Iterator &it) const{
    return (_data[it.Right()]-2*_data[it]+_data[it.Left()])/(((_geom->Mesh())[0])*((_geom->Mesh())[1]));
}
/// Computes the central difference quatient of 2nd order in y-dim at [it]
real_t Grid::dyy(const Iterator &it) const{
    return (_data[it.Top()]-2*_data[it]+_data[it.Down()])/(((_geom->Mesh())[1])*((_geom->Mesh())[1]));
}

/// Computes u*du/dx with the donor cell method
real_t Grid::DC_udu_x(const Iterator &it, const real_t &alpha) const{

    real_t DC_udu_x_var, left_var,right_var;
    left_var= (((_data[it]+_data[it.Right()])/2)*((_data[it]+_data[it.Right()])/2)-((_data[it]+_data[it.Left()])/2)*((_data[it]+_data[it.Left()])/2))/ ((_geom->Mesh())[0]);
    right_var=alpha*(-std::fabs((_data[it]+_data[it.Right()])/2)*dx_r(it)/2+std::fabs((_data[it]+_data[it.Left()])/2)*dx_l(it)/2)/ ((_geom->Mesh())[0]);


     return left_var+right_var;
}
/// Computes v*du/dy with the donor cell method
real_t DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const;
/// Computes u*dv/dx with the donor cell method
real_t DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const;
/// Computes v*dv/dy with the donor cell method
real_t DC_vdv_y(const Iterator &it, const real_t &alpha) const;

///implement all other derivatives

real_t Grid::Max() const
{
    real_t max = _data[0];
    for(int i=0; i<_geom->Size()[0]*_geom->Size()[1]; i++)
    {if (_data[i]>max) max = _data[i];}
    return max;
}

real_t Grid::Min() const
{
    real_t min = _data[0];
    for(int i=0; i<_geom->Size()[0]*_geom->Size()[1]; i++)
    {if (_data[i]<min) min = _data[i];}
    return min;
}
