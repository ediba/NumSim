#include "grid.hpp"
#include "geometry.hpp"
#include "iterator.hpp"
#include <cmath>        // std::abs, pow
#include <iostream>



Grid::Grid(const Geometry *geom, const multi_real_t &offset)
 {
     _geom=geom;
     _offset=offset;
    ///plus 2 ???
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
//Probs mit size() wenn es nur interior zellen enthält muss noch +2 hin
index_t Grid::IterFromPos(const multi_index_t &pos) const{
    index_t ind;
    ind = pos[1]*(_geom->Size()[0]+2)+ pos[0];
    return ind;
}
///Neuere interpolate
//TODO: aufpassen mit Ursprung und wenn negative positionen eingefordert werden kann es zu komplikationen kommen..
real_t Grid::Interpolate(const multi_real_t &pos) const{
    //std::cout << "begin interpolate" << std::endl;
    multi_index_t coordinates;
    real_t absX;
    real_t relX;
    real_t absY;
    real_t relY;
    real_t valueX1;
    real_t valueX2;
   
    
    //std::cout << "\tInterpolation position: " << pos[0] << "|" << pos[1] << std::endl;
    coordinates[0] = (index_t) (((pos[0]-_offset[0])/(_geom->Mesh()[0]))+1); //alte Version: (pos[0]-fmod(pos[0], _geom->Mesh()[0]))/_geom->Mesh()[0];
    coordinates[1] = (index_t) (((pos[1]-_offset[1])/(_geom->Mesh()[1]))+1); //(pos[1]-fmod(pos[1], _geom->Mesh()[1]))/_geom->Mesh()[1];
    //std::cout << "\t" << coordinates[0] << ", " << coordinates[1] << std::endl;
    Iterator it(_geom, IterFromPos(coordinates));
    
    //std::cout << "\tIterator constructed value = " << it.Value() << std::endl;
    
    absX = fmod(pos[0]-_offset[0]+_geom->Mesh()[0], _geom->Mesh()[0]);
    absY = fmod(pos[1]-_offset[1]+_geom->Mesh()[1], _geom->Mesh()[1]);
    //std::cout <<"\t"<< absX << ", " <<absY << std::endl;
    
    relX = absX/_geom->Mesh()[0];
    relY = absY/_geom->Mesh()[1];
    //std::cout << "\t" << relX << ", " <<relY << std::endl;
    
    valueX1 = ((1-relX)*_data[it]+(relX)*_data[it.Right()]);
    
    //std::cout << "\toffset = " << _offset[0] << " " << _offset [1] << std::endl;
    //std::cout << "Indices : P1  = " << it.Value() << "Koordinaten: "<< it.Pos()[0]*_geom->Mesh()[0] <<" "<<it.Pos()[1]*_geom->Mesh()[1] << std::endl;
    it = it.Top();
    valueX2 = (1-relX) * _data[it] + relX * _data[it.Right()];
    return (valueX1 * (1-relY) + valueX2 * (relY));
    
    
}
// real_t Grid::Interpolate(const multi_real_t &pos) const{
//     
//     Iterator it(_geom);
//     it.First();
//     
//     index_t cellMin_ = 0;
//     real_t absX;
//     real_t relX;
//     real_t absY;
//     real_t relY;
//     
//     real_t valueX1;
//     real_t valueX2;
//     
//     
//     //loop durch alle zellen und suchen nach den 4 zelle mit dem kleinsten abstand von pos
//     //setzt vorraus, dass die Funktion in iteraror::Pos() echte koordinaten zurückgibt 
//     // die funktion soll aber integer zurückgeben deswegen weiß ichs nicht obs stimmt
//     //offset muss noch richtig mit einbezogen werden
//     
//     //zuerst loop durch unterste reihe, um kleinsten Abstand x < Zellenbreite zu finden
//     while (it.Valid()){
//         absX = pos[0] - it.Pos()[0]*_geom->Mesh()[0] + _offset[0];
//         if( absX < _geom->Mesh()[0] ){
//             
//             cellMin_ = it.Value();
//             break;
//         }
//         it.Next();
//     }
//     
//     //wenn der erste x-Abstand < Zellenbreite gefunden ist, wird in y richtung nach oben nach dem y abstand < Zellenhöhe gesucht
//     while (it.Valid()){
//         absY = pos[1] - it.Pos()[1]*_geom->Mesh()[1] + _offset[1];
//         if( absY < _geom->Mesh()[1]){
//             cellMin_ = it.Value();
//             break;
//         }
//         it = it.Top();
//     }
//     // ab jetzt müsste die linke untere Zelle in it vermerkt sein also wird mit den restlichen 3 Zellen interpoliert
//     // jetzt wird interpoliert mit den vier umgebenden Zellen
//     relX = absX/_geom->Mesh()[0];
//     relY = absY/_geom->Mesh()[1];
//     
//     // das ist richtig, falls sobald it.Left() aufggerufen wird sich it ändert
//     // wenn it erstmal gleich bleibt muss es geändert werden.
//     
//     valueX1 = ((1-relX)*_data[it]+(relX)*_data[it.Right()]);
//     
//     std::cout << "offset = " << _offset[0] << " " << _offset [1] << std::endl;
//     std::cout << "Indices : P1  = " << it.Value() << "Koordinaten: "<< it.Pos()[0]*_geom->Mesh()[0] <<" "<<it.Pos()[1]*_geom->Mesh()[1] << std::endl;
//     it = it.Top();
//     valueX2 = (1-relX) * _data[it] + relX * _data[it.Right()];
//     return (valueX1 * (1-relY) + valueX2 * (relY));
// }
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

    real_t left_var,right_var;
    left_var= (((_data[it]+_data[it.Right()])/2)*((_data[it]+_data[it.Right()])/2)-((_data[it]+_data[it.Left()])/2)*((_data[it]+_data[it.Left()])/2))/ ((_geom->Mesh())[0]);
    right_var=alpha*(std::fabs((_data[it]+_data[it.Right()])/2)*((_data[it]-_data[it.Right()])/2)-std::fabs((_data[it]+_data[it.Left()])/2)*((_data[it.Left()]-_data[it])/2))/ ((_geom->Mesh())[0]);


     return left_var+right_var;
}


/// Computes v*du/dy with the donor cell method
real_t Grid::DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const{
    real_t left_var, right_var;
    
    //TODO Ich glaube man braucht nur einen Iterator, sonst wird für v immmer nur links unten verwendet
    //habe es jetzt mal so geändert, wie ich glaub, dass es sein müsste
    
//     InteriorIterator vIter(_geom);
//     vIter.First();
//     
//     
//     left_var = (((v->_data[vIter]+v->_data[vIter.Right()])/2)*((_data[it]+_data[it.Top()])/2)-((v->_data[vIter.Down()]+ v->_data[vIter.Right().Down()])/2)*((_data[it.Down()]+_data[it])/2))/ ((_geom->Mesh())[1]);
//     right_var=alpha*((std::fabs(v->_data[vIter]+v->_data[vIter.Right()])/2)*((_data[it]-_data[it.Top()])/2)-(std::fabs(v->_data[vIter.Down()]+v->_data[vIter.Right().Down()])/2)*((_data[it.Down()]-_data[it])/2)) / ((_geom->Mesh())[1]);

    left_var = (((v->_data[it]+v->_data[it.Right()])/2)*((_data[it]+_data[it.Top()])/2)-((v->_data[it.Down()]+ v->_data[it.Right().Down()])/2)*((_data[it.Down()]+_data[it])/2))/ ((_geom->Mesh())[1]);
    
    right_var=alpha*((std::fabs(v->_data[it]+v->_data[it.Right()])/2)*((_data[it]-_data[it.Top()])/2)-(std::fabs(v->_data[it.Down()]+v->_data[it.Right().Down()])/2)*((_data[it.Down()]-_data[it])/2)) / ((_geom->Mesh())[1]);
    
    return left_var+right_var;

}

/// Computes u*dv/dx with the donor cell method
real_t Grid::DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const{
    
    real_t left_var, right_var;
    //TODO Ich glaube man braucht nur einen Iterator, sonst wird für v immmer nur links unten verwendet
    //habe es jetzt mal so geändert, wie ich glaub, dass es sein müsste
    
//     InteriorIterator uIter(_geom);
//     uIter.First();
//     left_var = (((u->_data[uIter]+ u->_data[uIter.Top()])/2)*((_data[it]+_data[it.Right()])/2)-((u->_data[uIter.Left()]+ u->_data[uIter.Left().Top()])/2)*((_data[it.Left()]+_data[it])/2))/ ((_geom->Mesh())[0]);
//     right_var=alpha*((std::fabs(u->_data[uIter]+ u->_data[uIter.Top()])/2)*((_data[it.Right()]-_data[it])/2)-(std::fabs(u->_data[uIter.Left()]+ u->_data[uIter.Left().Top()])/2)*((_data[it]-_data[it.Left()])/2)) / ((_geom->Mesh())[0]);
    
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


///implement all other derivatives

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
