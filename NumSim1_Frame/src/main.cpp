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
#include "iterator.hpp"

int main(int argc, char **argv) {
  // Create parameter and geometry instances with default values
  Parameter param;
  Geometry geom;
  //Renderer renderer;
  // Create the fluid solver
  Compute comp(&geom, &param);
  
  param.Load("default.param");
  geom.Load("default.geom");
  
  multi_index_t size = geom.Size();
  std::cout << "Size = "<< size[0] <<" " <<size[1] <<std::endl;
    multi_real_t length = geom.Length();
    std::cout << "Length = "<< length[0] <<" " <<length[1] <<std::endl;
    multi_real_t mesh = geom.Mesh();
    std::cout << "Mesh = "<< mesh[0] << " " << mesh[1] << std::endl;
    
  
    multi_real_t offset = {0.5, 0.5};
    Grid testgrid = Grid(&geom, offset);
//     testgrid.Initialize(2);
//     InteriorIterator iter(&geom);
//     testgrid.Cell(iter) = 0;
//     multi_real_t InterX = {1.5, 1.5};
//     real_t TestWert = testgrid.Interpolate(InterX);
//     
//     std::cout << "Interpoliert an der Stelle "<< InterX[0] << "/"<< InterX[1]<< " = " <<TestWert<<std::endl;
    
   
//    const Grid *visugrid;
//   bool run = true;

//    visugrid = comp.GetVelocity();

  //visualize testgrid
//     Renderer renderer = Renderer(length, mesh);
//     renderer.Init(200,200,100);
//     renderer.Render(&testgrid);
//     renderer.ShowGrid(true);
//     std::cin.get();
//     
// 
//     
    //test iterator
    Grid testgrid2 = Grid(&geom,{0.5,0.5});
    testgrid2.Initialize(1);
    //Iterator test2iterator(&geom,0);
    //InteriorIterator test2iterator(&geom);
    BoundaryIterator test2iterator(&geom);
    test2iterator.SetBoundary(3);
    test2iterator.First();
    testgrid2.Cell(test2iterator) = 0;
    //testgrid2.Cell(test2iterator) = 1;
    Renderer renderer2 = Renderer(length, mesh);
    renderer2.Init(500,500,0);
    renderer2.Render(&testgrid2);
    renderer2.ShowGrid(true);
    real_t t;
    while (test2iterator.Valid()){
        std::cout << "Iterator value: " << test2iterator.Value() << std::endl;
        std::cout << "\t left = " <<  testgrid2.Cell(test2iterator.Left()) << " mid = "<<  testgrid2.Cell(test2iterator) << " right = " <<  testgrid2.Cell(test2iterator.Right()) << std::endl;
        /*std::cout << "Unterste Zeile: ";
        for(int j=0; j<5; j++){
            std::cout << testgrid2.Interpolate({0.5+j,0.5}) << "  ";
        }
        std::cout << std::endl;*/
        std::cin.get();
        testgrid2.Cell(test2iterator) = 1;
        test2iterator.Next();
        testgrid2.Cell(test2iterator) = 0;
        //t = testgrid2.Interpolate({0.0,0.5});
        //std::cout << " interpoliert an der stelle 0.0 0.5 = " << t << std::cout; 

        renderer2.Render(&testgrid2);
        
    }
//     for(int i=0; i<50; i++){
//         std::cout << "Iterator value: " << test2iterator.Value() << std::endl;
//         /*std::cout << "Unterste Zeile: ";
//         for(int j=0; j<5; j++){
//             std::cout << testgrid2.Interpolate({0.5+j,0.5}) << "  ";
//         }
//         std::cout << std::endl;*/
//         std::cin.get();
//         testgrid2.Cell(test2iterator) = 1;
//         test2iterator.Next();
//         testgrid2.Cell(test2iterator) = 0;
//         renderer2.Render(&testgrid2);
//     }
    
    // Run a few steps
//     for (uint32_t i = 0; i < 9; ++i)
//       comp.TimeStep(false);
//     comp.TimeStep(true);
  return 0;
}
