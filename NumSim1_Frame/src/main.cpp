/*
 *  Copyright (C) 2015   Malte Brunn
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "typedef.hpp"
#include "compute.hpp"
#include "geometry.hpp"
#include "parameter.hpp"
#include "grid.hpp"
#include "visu.hpp"
#include "visu.cpp"
//#include "vtk.hpp"


int main(int argc, char **argv) {
  // Create parameter and geometry instances with default values
  Parameter param;
  Geometry geom;
  //Renderer renderer;
  // Create the fluid solver
  Compute comp(&geom, &param);
  
  param.Load("default.param");
  geom.Load("default.geom");
  Grid testgrid = Grid(&geom);
  multi_index_t size = geom.Size();
  std::cout << "Size = "<< size[0] <<" " <<size[1] <<std::endl;
    multi_real_t length = geom.Length();
    std::cout << "Length = "<< length[0] <<" " <<length[1] <<std::endl;
    multi_real_t mesh = geom.Mesh();
    std::cout << "Mesh = "<< mesh[0] << " " << mesh[1] << std::endl;
    
    
    testgrid.Initialize(2);
    
    
    multi_real_t InterX = {0.5, 1};
    real_t TestWert = testgrid.Interpolate(InterX);
    
    std::cout << "Interpoliert an der Stelle "<< InterX[0] << "/"<< InterX[1]<< " = " <<TestWert<<std::endl;
    
   
    const Grid *visugrid;
//   bool run = true;

    visugrid = comp.GetVelocity();

  //visualize testgrid
    Renderer renderer = Renderer(length, mesh);
    renderer.Init(200,200,100);
    renderer.Render(&testgrid);
    renderer.ShowGrid(true);
    std::cin.get();
    
    // Run a few steps
//     for (uint32_t i = 0; i < 9; ++i)
//       comp.TimeStep(false);
//     comp.TimeStep(true);
  return 0;
}
