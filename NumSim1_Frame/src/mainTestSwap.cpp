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
    //Communicator:
    const Communicator comm(&argc, &argv);
    std::cout << " ich bin " << comm.ThreadNum() << " meine Position ist " << comm.ThreadIdx()[0] << " " << comm.ThreadIdx()[1]<< " die Groesse ist:  "<< comm.ThreadDim()[0] << " "<<  comm.ThreadDim()[1] <<" Ich bin " << comm.EvenOdd() << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Communicator constructed" << std::endl;
    
  Parameter param;
  Geometry geom(&comm);
  
  Grid Testgrid(&geom);
  ////////////////////////////////////////////////
  // Test für comm.copyBoundary
  // im input file kleine anzahl an gitter nehmen!!
  ///////////////////////////////////////////////
  //Intialisiere
  Testgrid.Initialize(comm.ThreadNum());
  MPI_Barrier(MPI_COMM_WORLD);
  //Gib Koordinaten für jeden Prozess aus
//   for(int i=0; i<comm.ThreadCnt();i++){
//       if(comm.ThreadNum() == i){
//           std::cout << " ich bin Prozess " << comm.ThreadNum() << " Mein Position ist " <<comm.ThreadIdx()[0]<<"|"<<comm.ThreadIdx()[1]<< std::endl;
//       }
//       MPI_Barrier(MPI_COMM_WORLD);
//   }
  //Gitter am Anfang ausgeben
  MPI_Barrier(MPI_COMM_WORLD);
  for(int i=0; i<comm.ThreadCnt();i++){
      if(comm.ThreadNum() == i){
          std::cout << " ich bin Prozess " << comm.ThreadNum() << " Mein grid vor dem Swap " << std::endl;
          Testgrid.PrintGrid();
      }
      MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  //Swap durchführen
  comm.copyBoundary(&Testgrid);
  MPI_Barrier(MPI_COMM_WORLD);
  //Gitter nach dem Swap ausgeben
  for(int i=0; i<comm.ThreadCnt();i++){
      if(comm.ThreadNum() == i){
          std::cout << " ich bin Prozess " << comm.ThreadNum() << " Mein grid nach dem Swap " << std::endl;
          Testgrid.PrintGrid();
          MPI_Barrier(MPI_COMM_WORLD);
      }
      MPI_Barrier(MPI_COMM_WORLD);
  }
  MPI_Barrier(MPI_COMM_WORLD);
  
  //std::cout<<" \n" << std::endl;
  //MPI_Barrier(MPI_COMM_WORLD);
  
//   std::cout << "Size = " << geom.Size()[0] << " " << geom.Size()[1] << std::endl;
//   // Create the fluid solver
//   std::cout << "compute start" << std::endl;
//   Compute comp(&geom, &param);
//   std::cout << "compute done" << std::endl;
// 
//   // Create and initialize the visualization
//   Renderer visu(geom.Length(), geom.Mesh());
//     std::cout << "Renderer done" << std::endl;
//   visu.Init(800, 800);
//     std::cout << "Renderer done" << std::endl;
// 
//   // Create a VTK generator
//   VTK vtk(geom.Mesh(), geom.Size());
// 
//   const Grid *visugrid;
//   bool run = true;
// 
//     while (comp.GetTime() < 50 && run) {
//         visugrid = comp.GetVelocity();
//         // Render and check if window is closed
//         int key = visu.Check();
//         visu.Render(visugrid);//, visugrid->Min(), visugrid->Max());
//         if (key == 10) {
//             //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
//             comp.TimeStep(true);
//         }
//         if (key == -1) {
//             run = false;
//         }
//     }
    
  return 0;
}
