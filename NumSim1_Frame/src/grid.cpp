#include "grid.hpp"

//Ich weiß noch nicht so richtig wie man auf die tasächlichen Werte wie u oder v zugreift und
// wo die gespeichert sind, deswegen hab ich jetzt mal IrgendeinWert() eingefügt, wo denke ich dann
// der eigentliche wert rausgegeben wird

/// Constructs a grid based on a geometry
Grid::Grid(const Geometry *geom){
    //TODO
    _data = new real_t ((geom->Size()[0])*(geom->Size()[1]));
    _offset[0] = 0;
    _offset[1] = 0;
    _geom = geom;
}

/// Constructs a grid based on a geometry with an offset
// @param geom   Geometry information
// @param offset distance of staggered grid point to cell's anchor point;
//               (anchor point = lower left corner)

// Grid wird mit (n+1)*(m+1) erstellt also mit ghost zellen
// bin mir nicht sicher ob korrekt bzw. wieviele ghost zellen es sein sollen
Grid::Grid(const Geometry *geom, const multi_real_t &offset){
    //TODO
    _data = new real_t ((geom->Size()[0])*(geom->Size()[1]));
    _offset[0] = offset[0];
    _offset[1] = offset[1];
    _geom = geom;
}

/// Deletes the grid
Grid::~Grid(){
    //TODO
}

///     Initializes the grid with a value
void Grid::Initialize(const real_t &value){
    //TODO
    Iterator it(_geom);
    it.First();
    while (it.Valid()){
        _data[it] = value;
        it.Next();
    }
    
}

/// Write access to the grid cell at position [it]

//Verstehe die beiden nicht wirklich einmal read und einmal write mit gleichem name??
real_t &Grid::Cell(const Iterator &it){
    //TODO weiß nicht genau was das machen soll..
     return this->_data[it];
}
/// Read access to the grid cell at position [it]
const real_t &Grid::Cell(const Iterator &it) const {
    return _data[it];
    
}

/// Interpolate the value at a arbitrary position
// real_t Grid::Interpolate(const multi_real_t &pos) const{
//     
//     Iter it(_geom);
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
//         absX = pos[0] - it.Pos()[0] + _offset[0];
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
//         absY = pos[1] - it.Pos()[1] + _offset[1];
//         if( absX < _geom->Mesh()[1]){
//             cellMin_ = it.Value();
//             break;
//         }
//         it.Top();
//     }
//     // ab jetzt müsste die linke untere Zelle in it vermerkt sein also wird mit den restlichen 3 Zellen interpoliert
//     // jetzt wird interpoliert mit den vier umgebenden Zellen
//     relX = absX/_geom->Mesh()[0];
//     relY = absY/_geom->Mesh()[1];
//     
//     // das ist richtig, falls sobald it.Left() aufggerufen wird sich it ändert
//     // wenn it erstmal gleich bleibt muss es geändert werden.
//     
//     valueX1 = (relX*_data[it]+(1-relX)*_data[it.Right()]);
//      valueX2 = (1-relX) * _data[it.Top()] + relX * _data[it.Left()];
//     
//     return (valueX1 * relY + valueX2 * (1-relY));
// }

/// Computes the left-sided difference quatient in x-dim at [it]
real_t Grid::dx_l(const Iterator &it) const{
    return ( _data[it] - _data[it.Left()] )/_geom->Mesh()[0];
}
/// Computes the right-sided difference quatient in x-dim at [it]
real_t Grid::dx_r(const Iterator &it) const{
    return (_data[it.Right()] - _data[it])/_geom->Mesh()[0];
}
/// Computes the left-sided difference quatient in y-dim at [it]
// maybe better down-sided wegen y
real_t Grid::dy_l(const Iterator &it) const{
    return (_data[it] - _data[it.Down()])/_geom->Mesh()[1];
}
/// Computes the right-sided difference quatient in x-dim at [it]
//maybe better up sided wegen y
real_t Grid::dy_r(const Iterator &it) const{
    return (_data[it.Top()] - _data[it])/_geom->Mesh()[1];
}
/// Computes the central difference quatient of 2nd order in x-dim at [it]
real_t Grid::dxx(const Iterator &it) const{
    return (dx_r(it) - dx_l(it))/_geom->Mesh()[0];
}
/// Computes the central difference quatient of 2nd order in y-dim at [it]
real_t Grid::dyy(const Iterator &it) const{
    return (dy_r(it) - dy_l(it))/_geom->Mesh()[1];
}


/// Computes u*du/dx with the donor cell method
real_t Grid::DC_udu_x(const Iterator &it, const real_t &alpha) const{
}

/// Computes v*du/dy with the donor cell method
real_t Grid::DC_vdu_y(const Iterator &it, const real_t &alpha, const Grid *v) const{
}

/// Computes u*dv/dx with the donor cell method
real_t Grid::DC_udv_x(const Iterator &it, const real_t &alpha, const Grid *u) const{
}
/// Computes v*dv/dy with the donor cell method
real_t Grid::DC_vdv_y(const Iterator &it, const real_t &alpha) const{
}

/// Returns the maximal value of the grid
// real_t Grid::Max() const{
//     Iterator it(_geom);
//     it.First();
//     
//     reatl_t maxVal = _data[it];
//     it.Next();
//     
//     while (iter.Valid())
//     {
//         maxVal = max(maxVal, _data[it]);
//     }
//     return maxVal;
// }
/// Returns the minimal value of the grid
// real_t Grid::Min() const{
//     Iterator it(_geom);
//     it.First();
//     
//     reatl_t minVal = _data[it];
//     it.Next();
//     
//     while (iter.Valid())
//     {
//         minVal = min(minVal, _data[it]);
//     }
//     return minVal;
// }
/// Returns the absolute maximal value
// real_t Grid::AbsMax() const{
//     
//     Iterator it(_geom);
//     it.First();
//     
//     reatl_t maxValAbs = fabs(_data[it]);
//     it.Next();
//     
//     while (iter.Valid())
//     {
//         maxValAbs = max(fabs(maxValAbs), fabs(_data[it]));
//     }
//     return maxValAbs;
// }

/// Returns a pointer to the raw data
real_t *Grid::Data(){
    return _data;
}