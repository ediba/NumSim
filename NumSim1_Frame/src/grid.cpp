#include "grid.hpp"
#include "geometry.hpp"
#include "iterator.hpp"
#include <cmath>
#include <iostream>



Grid::Grid(const Geometry *geom, const multi_real_t &offset)
 {
     _geom=geom;
     _offset=offset;
     _data = new real_t[(_geom->Size()[0]+2) * (_geom->Size()[1]+2)];
  this->Initialize(0.0);
   std::cout << " Grid Initialized " <<std::endl;
}
Grid::Grid(const Geometry* geom)
{
    
     _geom=geom;
     _offset={0.0,0.0};
    ///plus 2 ???
    _data = new real_t[(_geom->Size()[0]+2) * (_geom->Size()[1]+2)];
    this->Initialize(0.0);
    std::cout << " Grid Initialized " <<std::endl;
    
}
Grid::~Grid()
{delete[] _data;}

void Grid::Initialize(const real_t& value)
{
    for(int i=0; i<(_geom->Size()[0]+2)*(_geom->Size()[1]+2); i++)
    {_data[i] = value;}
}

real_t &Grid::Cell(const Iterator &it){
  return _data[it.Value()];
}


const real_t &Grid::Cell(const Iterator &it) const{
  return _data[it.Value()];
}
///returns the iterator for one coordinate 
index_t Grid::IterFromPos(const multi_index_t &pos) const{
    index_t ind;
    ind = pos[1]*(_geom->Size()[0]+2)+ pos[0];
    return ind;
}

real_t Grid::Interpolate(const multi_real_t &pos) const{
    //std::cout << "begin interpolate" << std::endl;
    multi_index_t coordinates;
    real_t absX;
    real_t relX;
    real_t absY;
    real_t relY;
    real_t valueX1;
    real_t valueX2;
   
    coordinates[0] = (index_t) (((pos[0]-_offset[0])/(_geom->Mesh()[0]))+1); 
    coordinates[1] = (index_t) (((pos[1]-_offset[1])/(_geom->Mesh()[1]))+1); 
    Iterator it(_geom, IterFromPos(coordinates));

    absX = fmod(pos[0]-_offset[0]+_geom->Mesh()[0], _geom->Mesh()[0]);
    absY = fmod(pos[1]-_offset[1]+_geom->Mesh()[1], _geom->Mesh()[1]);
    
    relX = absX/_geom->Mesh()[0];
    relY = absY/_geom->Mesh()[1];
    
    valueX1 = ((1-relX)*_data[it]+(relX)*_data[it.Right()]);
    
    it = it.Top();
    valueX2 = (1.0-relX) * _data[it] + relX * _data[it.Right()];
    return (valueX1 * (1.0-relY) + valueX2 * (relY));
    
    
}

real_t Grid::dx_l(const Iterator& it) const
{return (_data[it] - _data[it.Left()])/((_geom->Mesh())[0]);}

real_t Grid::dx_r(const Iterator& it) const
{return (_data[it.Right()] - _data[it])/((_geom->Mesh())[0]);}

real_t Grid::dy_l(const Iterator& it) const
{return (_data[it] - _data[it.Down()])/((_geom->Mesh())[1]);}

real_t Grid::dy_r(const Iterator& it) const
{return (_data[it.Top()] - _data[it])/((_geom->Mesh())[1]);}

real_t Grid:: dxx(const Iterator &it) const{
    return (_data[it.Right()]-2.0*_data[it]+_data[it.Left()])/(((_geom->Mesh())[0])*((_geom->Mesh())[0]));
}
/// Computes the central difference quatient of 2nd order in y-dim at [it]
real_t Grid::dyy(const Iterator &it) const{
    return (_data[it.Top()]-2.0*_data[it]+_data[it.Down()])/(((_geom->Mesh())[1])*((_geom->Mesh())[1]));
}

/// Computes u*du/dx with the donor cell method
real_t Grid::DC_udu_x(const Iterator &it, const real_t &alpha) const{

    real_t left_var,right_var;
    left_var= (((_data[it]+_data[it.Right()])/2)*((_data[it]+_data[it.Right()])/2)-((_data[it]+_data[it.Left()])/2)*((_data[it]+_data[it.Left()])/2))/ ((_geom->Mesh())[0]);
    right_var=alpha*(std::fabs((_data[it]+_data[it.Right()])/2)*((_data[it]-_data[it.Right()])/2)-std::fabs((_data[it]+_data[it.Left()])/2)*((_data[it.Left()]-_data[it])/2))/ ((_geom->Mesh())[0]);


     return left_var+right_var;
}


/// Computes v*du/dy with the donor cell method
real_t Grid::DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const{
    real_t left_var, right_var;

    left_var = (((v->_data[it]+v->_data[it.Right()])/2)*((_data[it]+_data[it.Top()])/2)-((v->_data[it.Down()]+ v->_data[it.Right().Down()])/2)*((_data[it.Down()]+_data[it])/2))/ ((_geom->Mesh())[1]);
    
    right_var=alpha*((std::fabs(v->_data[it]+v->_data[it.Right()])/2)*((_data[it]-_data[it.Top()])/2)-(std::fabs(v->_data[it.Down()]+v->_data[it.Right().Down()])/2)*((_data[it.Down()]-_data[it])/2)) / ((_geom->Mesh())[1]);
    
    return left_var+right_var;

}

/// Computes u*dv/dx with the donor cell method
real_t Grid::DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const{
    
    real_t left_var, right_var;
    
     left_var = (((u->_data[it]+ u->_data[it.Top()])/2)*((_data[it]+_data[it.Right()])/2)-((u->_data[it.Left()]+ u->_data[it.Left().Top()])/2)*((_data[it.Left()]+_data[it])/2))/ ((_geom->Mesh())[0]);
     
     
    right_var=alpha*((std::fabs(u->_data[it]+ u->_data[it.Top()])/2)*((_data[it.Right()]-_data[it])/2)-(std::fabs(u->_data[it.Left()]+ u->_data[it.Left().Top()])/2)*((_data[it]-_data[it.Left()])/2)) / ((_geom->Mesh())[0]);

    return left_var+right_var;
}


/// Computes v*dv/dy with the donor cell method
real_t Grid::DC_vdv_y(const Iterator &it, const real_t &alpha) const{

    real_t left_var,right_var;
    left_var= (((_data[it]+_data[it.Top()])/2)*((_data[it]+_data[it.Top()])/2)-((_data[it.Down()]+_data[it])/2)*((_data[it.Right()]-_data[it])/2))/ ((_geom->Mesh())[1]);
    right_var=alpha*(std::fabs((_data[it]+_data[it.Top()])/2)*((_data[it.Right()]-_data[it])/2)-std::fabs((_data[it]+_data[it.Down()])/2)*((_data[it]-_data[it.Left()])/2))/ ((_geom->Mesh())[1]);

     return left_var+right_var;

}
/// Returns the absolute maximal value
real_t Grid::AbsMax() const{
    
    Iterator it(_geom);
    it.First();
    
    real_t maxAbs = fabs(_data[it]);
    it.Next();
    
    while (it.Valid())
    {
        maxAbs = std::max(fabs(maxAbs), fabs(_data[it]));
        it.Next();
    }
    return maxAbs;
}

real_t Grid::Max() const
{
    Iterator it (_geom);
    real_t max = _data[it];
    it.Next();
    while (it.Valid()){
        if(_data[it]>max){
            max = _data[it];
        }
        it.Next();
    }
    return max;
}

real_t Grid::Min() const
{
    Iterator it (_geom);
    real_t min = _data[it];
    it.Next();
    while (it.Valid()){
        if(_data[it]<min){
            min = _data[it];
        }
        it.Next();
    }
    return min;
}
/// Prints Grid to console
void Grid::PrintGrid() const{
    for(index_t y=_geom->Size()[1]+1; y<_geom->Size()[1]+2;y--){
        for(index_t x=0; x<_geom->Size()[0]+2;x++){
            Iterator it(_geom,IterFromPos({x,y}));
            std::cout<< Cell(it) <<"\t";
        }
        std::cout<<std::endl;
    }
}