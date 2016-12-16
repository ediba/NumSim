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
#include "iterator.hpp"
#include "grid.hpp"

#include <vector>
#include <sstream>
#include <fstream>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include "comm.hpp"

#include <cstdio>
#include <cstring>
#include <cstdlib>

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
    //Alt
  //Geometry();
  //Neuer Constructor
    Geometry(const Communicator* comm, const char * geometryInput);
  
  
  

  /// Loads a geometry from a file
  void Load(const char *file);
    
  /// Returns the number of cells in each dimension
  const multi_index_t &Size() const;
  
  ///Returns the total size of the domain
  const multi_index_t& TotalSize() const;
  
  /// Returns the length of the domain
  const multi_real_t &Length() const;
  
  /// Returns the total length of the domain
  const multi_real_t& TotalLength () const;
    
  /// Returns the meshwidth
  const multi_real_t &Mesh() const;

  /// Updates the velocity field u
  void Update_U(Grid *u) const;
  
  /// Updates the velocity field v
  void Update_V(Grid *v) const;
  
  /// Updates the pressure field p
  void Update_P(Grid *p) const;
  
  ///Für mpi gets the size of the threads
  void GetSizesOfThreads ();
  
  ///Für freie Geometries
  bool FreeGeometry();
  const index_t NumInteriorBounds() const;
  void Update_U_free(Grid* u)const;
  void Update_V_free(Grid *v)const;
  void Update_P_free(Grid *p)const;
  char &Flag(const Iterator &it);
    const char &Flag(const Iterator &it) const;

private:
    void Set_U_vertical(Iterator it, Grid* u)const;
    void Set_U_horizontal(Iterator it, Grid* u)const;
    void Set_U_slipV(Iterator it, Grid* u)const;
    void Set_U_slipH(Iterator it, Grid *u)const;
    void Set_U_noslip(Iterator it, Grid* u)const;
    void Set_U_outflow(Iterator it, Grid *u)const;
    void Set_U_inflow(Iterator it, Grid *u)const;
    
    void Set_V_vertical(Iterator it, Grid* v)const;
    void Set_V_horizontal(Iterator it, Grid* v)const;
    void Set_V_slipV(Iterator it, Grid* v)const;
    void Set_V_slipH(Iterator it, Grid *v)const;
    void Set_V_noslip(Iterator it, Grid* v)const;
    void Set_V_outflow(Iterator it, Grid *v)const;
    void Set_V_inflow(Iterator it, Grid *v)const;
    
    void Set_P_vertical(Iterator it, Grid* p)const;
    void Set_P_horizontal(Iterator it, Grid* p)const;
    void Set_P_slipV(Iterator it, Grid* p)const;
    void Set_P_slipH(Iterator it, Grid *p)const;
    void Set_P_noslip(Iterator it, Grid* p)const;
    void Set_P_outflow(Iterator it, Grid *p)const;
    void Set_P_inflow(Iterator it, Grid *p)const;
    
  const Communicator* _comm;
  char* _flag;
  bool _freeGeom;
  index_t _numInteriorBounds;
  multi_index_t _bsize;
  multi_real_t _blength;
  multi_index_t _size;
  multi_real_t _length;
  multi_real_t _h;
  multi_real_t _mesh;
  multi_real_t _velocity;
  real_t _pressure;
};
//------------------------------------------------------------------------------
#endif // __GEOMETRY_HPP
