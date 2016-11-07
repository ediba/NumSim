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
{return (_data[it.Value()] - _data[it.Left().Value()])/((_geom->Mesh())[0]);}

real_t Grid::dx_r(const Iterator& it) const
{return (_data[it.Right().Value()] - _data[it.Value()])/((_geom->Mesh())[0]);}

real_t Grid::dy_l(const Iterator& it) const
{return (_data[it.Value()] - _data[it.Down().Value()])/((_geom->Mesh())[1]);}

real_t Grid::dy_r(const Iterator& it) const
{return (_data[it.Top().Value()] - _data[it.Value()])/((_geom->Mesh())[1]);}
///central difference
real_t Grid::dx_c(const Iterator& it) const
{
    return ((_data[it.Right().Value()] - _data[it.Value()])/((_geom->Mesh())[0])+(_data[it.Value()] - _data[it.Left().Value()])/((_geom->Mesh())[0]))*0.5;
}


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
