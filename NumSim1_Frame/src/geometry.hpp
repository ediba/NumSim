/*
 * Copyright (C) 2015   Malte Brunn
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "typedef.hpp"
#include <sstream>
#include <fstream>
#include <string>
//------------------------------------------------------------------------------
#ifndef __GEOMETRY_HPP
#define __GEOMETRY_HPP
//------------------------------------------------------------------------------
class Geometry {
public:
  /// Constructs a default geometry:
  // driven cavity with 128 x 128 grid, no-slip boundary conditions
  // as shown below
  //
  //      u=1, v=0
  //    -------------
  //    |           |
  // u=0|           |u=0
  // v=0|           |v=0
  //    |           |
  //    |           |
  //    -------------
  //      u=0, v=0
  Geometry(){
      _size[0] = 128;
      _size[1] = 128;
      _length[0] = 1.0;
      _length[1] = 1.0;
      double hx=_length[0] / _size[0];
      double hy= _length[1] / _size[1];
      _h = {hx, hy};
      _velocity[0] = 0.0;
      _velocity[1] = 0.0;
      _pressure = 0.1;
  }

  /// Loads a geometry from a file
  void Load(const char *file){

       std::ifstream infile(file);
       std::string line;
       std::getline(infile, line);
       std::istringstream iss(line);
       if (!(iss >> _size[0] >> _size[1])) {  } // error

    // process pair (a,b)


}
  /// Returns the number of cells in each dimension
  const multi_index_t &Size() const{
      return _size;
  }
  /// Returns the length of the domain
  const multi_real_t &Length() const{
      return _length;
  }
  /// Returns the meshwidth
  const multi_real_t &Mesh() const{
      return _h;
  }

  /// Updates the velocity field u
  void Update_U(Grid *u) const;
  /// Updates the velocity field v
  void Update_V(Grid *v) const;
  /// Updates the pressure field p
  void Update_P(Grid *p) const;

private:
  multi_index_t _size;
  multi_real_t _length;
  multi_real_t _h;

  multi_real_t _velocity;
  real_t _pressure;
};
//------------------------------------------------------------------------------
#endif // __GEOMETRY_HPP
