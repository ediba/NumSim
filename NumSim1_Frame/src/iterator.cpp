#include "iterator.hpp"
#include "geometry.hpp"


/// we decided that the iterations will go from left to right and
/// from the bottom to the top
Iterator::Iterator(const Geometry* geom, const index_t& value){
    _geom = geom;
    _value = value;
    _valid = true;    
}

Iterator::Iterator(const Geometry *geom)
{
    _geom = geom; _value = 0; _valid = true;
}
const index_t& Iterator::Value() const
{return _value;}

Iterator::operator const index_t& () const
{return _value;}

void Iterator::First()
{_value = 0;}

void Iterator::Next(){
if(_value == (_geom->Size()[0]+2)*(_geom->Size()[1]+2)-1){
    _valid = false;}
else{_value++;}    
}
bool Iterator::Valid() const
{return _valid;}


/// Calculates the values of x and y in the coordinate system
multi_index_t Iterator::Pos() const
{
    index_t x, y;
    x = _value % (_geom->Size()[0]+2);
    y = _value / (_geom->Size()[0]+2);
    return multi_index_t(x,y);
}


/// returns the left position of the iterator, and if it is on the boundary, it will
///return itself
Iterator Iterator::Left() const
{
   ///checking if it is at the left boundary
    if (Pos()[0] == 0){
        return Iterator(_geom, _value);
    }else
        return Iterator(_geom, _value - 1);

}

/// returns the up position of the iterator, and if it is on the boundary, it will
///return itself
Iterator Iterator::Right() const
{
    //checking if it is at the right boundary
    if (Pos()[0] == _geom->Size()[0]+2-1){
        return Iterator(_geom, _value);
    }else
        return Iterator(_geom, _value + 1);
}
/// returns the up position of the iterator, and if it is on the boundary, it will
///return itself
Iterator Iterator::Top() const
{
    //checking if it is at the top boundary
    if (Pos()[1] == _geom->Size()[1]+2-1){
        return Iterator(_geom, _value);
    }else
        return Iterator(_geom, _value + _geom->Size()[0]+2);
}


///returns the down position of the iterator, and if it is on the boundary, it will
///return itself
Iterator Iterator::Down() const
{
    //checking if it is at the lower boundary
    if (Pos()[1] == 0){
        return Iterator(_geom, _value);
    }else
        return Iterator(_geom, _value - (_geom->Size()[0]+2));
}


/** Iterator for interior cells
*/


///Implemented classes for InteriorIterator

InteriorIterator::InteriorIterator(const Geometry* geom): 
Iterator(geom){
    //Iterator(geom); //funktioniert bei mir nicht
    //_geom = geom; _value = geom->Mesh()[0]+1;;
    _valid=true;
    First();
    
}

void InteriorIterator::First()
{
    _value = _geom->Size()[0]+2 + 1;
    ///check if this really should be true?
    _valid=true;
}

void InteriorIterator::Next()
{
    if (!_valid) return;
    ///check if the position is already on the boundary of the y axis
        // falls an x boundary 
            if (Pos()[0] == _geom->Size()[0]+2-2){
            _value += 3;
            }
            // falls nicht einfach weiter
            else {
                _value +=1;
                
            }// maybe do this earlier?
            
            if (_value == ((_geom->Size()[0]+2)*(_geom->Size()[1]+1))-2){
            _valid = false;
            }
    
}
    /** Iterator for domain boundary cells.
    */

    ///implementation of the BoundaryIterator
      /// Constructs a new BoundaryIterator
    BoundaryIterator::BoundaryIterator(const Geometry *geom):
    Iterator(geom){
        _value = 0;
        _boundary = 0; 
        _valid=true;
    }

      /// Sets the boundary to iterate
      //boundary 0 are the 4 corner points; boundary 1 is the bottom row; boundary 2 is the left boundary;
      // boundary 3 is the top boundary; and boundary 4 is the left boundary
    void BoundaryIterator::SetBoundary(const index_t& boundary)
    {
        _boundary = boundary;
        First();
    }
    /// Sets the iterator to the first element
     void BoundaryIterator::First()
    {
        _valid = true;
        if (_boundary == 0 ){
            _value = 0;
        }
        if ( _boundary == 1){
            _value = 1;
        }
        
        else if(_boundary == 2){
            _value = 2*(_geom->Size()[0]+2)-1;
        }
        else if(_boundary == 3){
            _value = (_geom->Size()[1]+2-1)*(_geom->Size()[0]+2)+1;
        }
        if (_boundary == 4){
            _value = _geom->Size()[0]+2;
        }

     }
      /// Goes to the next element of the iterator, disables it if position is end
     void BoundaryIterator::Next()
    {
        //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
        if (_boundary == 0){
            if(_value == (_geom->Size()[0]+2)*(_geom->Size()[1]+2)-1){
                _valid = false;
            }
            else if(_value == 0){
                _value = _geom->Size()[0]+2-1;
            }
            else if(Pos()[0] == _geom->Size()[0]+2-1){
                _value = (_geom->Size()[0]+2)*(_geom->Size()[1]+1);
            }
            else {
              _value = (_geom->Size()[0]+2)*(_geom->Size()[1]+2)-1;
            }
                
        }
        else if (_boundary == 1|| _boundary == 3){
            if(Pos()[0] == _geom->Size()[0]+2-2){
                _valid = false;
            }
            else{
            _value += 1;
            }
        }
        else if  (_boundary == 2|| _boundary == 4){
            if(Pos()[1] == _geom->Size()[1]+2-2){
                _valid = false;
            }
            else{
                _value += _geom->Size()[0]+2;
            }
        }
        else{
            std::cout << " Error Set the Boundary first !! "<< std::endl;
        }
     }


