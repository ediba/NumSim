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
#include "vtk.hpp"
#include "solver.hpp"

// #include "typedef.hpp"
// #include "compute.hpp"
// #include "geometry.hpp"
// #include "parameter.hpp"
// #include "grid.hpp"
// #include "visu.hpp"
// #include "visu.cpp"
// #include "vtk.hpp"
// #include "iterator.hpp"
int main(int argc, char **argv) {
  // Create parameter and geometry instances with default values
    std::cout << "main start" << std::endl;
  Parameter param;
  Geometry geom;
  // Create the fluid solver
  std::cout << "compute start" << std::endl;
  Compute comp(&geom, &param);
  std::cout << "compute done" << std::endl;

//#ifdef USE_DEBUG_VISU
  // Create and initialize the visualization

  Renderer visu(geom.Length(), geom.Mesh());
    std::cout << "Renderer done" << std::endl;
  visu.Init(800, 800);
    std::cout << "Renderer done" << std::endl;
//#endif // USE_DEBUG_VISU

  // Create a VTK generator
  VTK vtk(geom.Mesh(), geom.Size());

  const Grid *visugrid;
  bool run = true;

  //visugrid = comp.GetP();

  // Run the time steps until the end is reached
//   std::cout << " COmpTime() = " << comp.GetTime() << std::endl;
//   std::cout << " param.Tend() = " << param.Tend() << std::endl;
//    while (comp.GetTime() < param.Tend() && run) {
//        std::cout << " while loop start" <<std::endl;
// // //#ifdef USE_DEBUG_VISU
//     // Render and check if window is closed
//     switch (visu.Render(visugrid)) {
//     case -1:
//       run = false;
//       break;
//     case 0:
//       visugrid = comp.GetVelocity();
//       break;
//     case 1:
//       visugrid = comp.GetU();
//       break;
//     case 2:
//       visugrid = comp.GetV();
//       break;
//     case 3:
//       visugrid = comp.GetP();
//       break;
//     default:
//       break;
//     };
//#endif // DEBUG_VIS
  visugrid = comp.GetP();
//while (comp.GetTime() < param.Tend() && run) {
    while (comp.GetTime() < 50 && run) {
    // Create a VTK File in the folder VTK (must exist)
    //std::cout <<" create VTK file start" << std::endl;
//     vtk.Init("VTK/field");
//     vtk.AddField("Velocity", comp.GetU(), comp.GetV());
//     vtk.AddScalar("Pressure", comp.GetP());
//     vtk.Finish();
    //std::cout <<" create VTK file end" << std::endl;

    //visugrid = comp.GetP();
    // Run a few steps
    //for (uint32_t i = 0; i < 10; ++i){
        
        // Render and check if window is closed
        int key = visu.Check();
        visu.Render(visugrid, visugrid->Min(), visugrid->Max());
        if (key == 10) {
            //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
            comp.TimeStep(true);
        }
        if (key == -1) {
            run = false;
        }
      //std::cout<< " step number " << i << std::endl;
      
    //}
    //visugrid = comp.GetU();
    //  visu.Render(visugrid);
    //  visugrid->PrintGrid();
    //  std::cin.get();
    //comp.TimeStep(true);
}

  /*  //unit test SOR solver
    multi_real_t offset = {0.5*geom.Mesh()[0],0.5*geom.Mesh()[1]};
    Grid testgrid5(&geom, offset);
    testgrid5.Initialize(0);
    Grid zeroGrid(&geom,offset);
    zeroGrid.Initialize(0);
    Iterator it5(&geom, 66);
    testgrid5.Cell(it5) = 1;
    visu.Render(&testgrid5);
    visu.Render(&testgrid5);
    testgrid5.PrintGrid();
    SOR sor(&geom, param.Omega());
    // Run the time steps until the end is reached
    while (run) {
        // Render and check if window is closed
        int key = visu.Check();
        visu.Render(&testgrid5, testgrid5.Min(), testgrid5.Max());
        if (key == 10) {
            printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
            testgrid5.PrintGrid();
        }
        if (key == -1) {
            run = false;
        }
    }*/
  return 0;
}
