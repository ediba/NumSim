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
    
    //Parameters
    const char *geometryInput;
    const char *paramInput;
    if(argc>2){
        geometryInput = argv[1];
        paramInput = argv[2];
    }
    else{
        geometryInput = "default.geom";
        paramInput = "default.param";
    }
    
    //Communicator:
    const Communicator comm(&argc, &argv);
    std::cout << " ich bin " << comm.ThreadNum() << " meine Position ist " << comm.ThreadIdx()[0] << " " << comm.ThreadIdx()[1]<< " die Groesse ist:  "<< comm.ThreadDim()[0] << " "<<  comm.ThreadDim()[1] <<" Ich bin " << comm.EvenOdd() << std::endl;
    MPI_Barrier(MPI_COMM_WORLD);
    std::cout << "Communicator constructed" << std::endl;
    
  Parameter param(paramInput);
  Geometry geom(&comm,geometryInput);
  
  multi_real_t offset = {0.5*geom.Mesh()[0],0.5*geom.Mesh()[1]};
  Grid testgrid(&geom, offset);
  
  /////////////////////////////////////////////////////
  //Test für CGSolver
  /////////////////////////////////////////////////////
  testgrid.Initialize(0);
  if(comm.ThreadIdx()[0] == 0 && comm.ThreadIdx()[1]==0){
      Iterator it(&geom, 96);
      testgrid.Cell(it)=1;
      /*if(comm.isTop()){
          std::cout<< "Positionierung oben" << std::endl;
      }
      if(comm.isRight()){
          std::cout<< "Positionierung rechts" << std::endl;
      }
      if(comm.isLeft()){
          std::cout<< "Positionierung links" << std::endl;
      }
      if(comm.isBottom()){
          std::cout<< "Positionierung unten" << std::endl;
      }*/
  }
  for(int i=0; i<comm.ThreadCnt();i++){
      if(comm.ThreadNum() == i){
          std::cout << " ich bin Prozess " << comm.ThreadNum() << " Mein grid vor SOR " << std::endl;
          testgrid.PrintGrid();
      }
      MPI_Barrier(MPI_COMM_WORLD);
  }
  Grid zerogrid(&geom);
  zerogrid.Initialize(0);
  CGSolver cgSolver(&geom, &comm);
  
  MPI_Barrier(MPI_COMM_WORLD);
  
    // Create and initialize the visualization
    Renderer visu(geom.Length(), geom.Mesh());
    //std::cout << "Renderer done" << std::endl;
    visu.Init(700, 700);
    //std::cout << "Renderer done" << std::endl;
    const Grid *visugrid;
    visugrid=&testgrid;
    bool run = true;
    int counter = 1;
    MPI_Barrier(MPI_COMM_WORLD);
    while (run) {
        // Render and check if window is closed
        int key = visu.Check();
        visu.Render(&testgrid);//, visugrid->Min(), visugrid->Max());
        if (key == 10) {
            //do RedOrBlackSOR cycle
            //std::cout<<"Cycle started" << std::endl;
            std::cout << counter <<":\tnew residual is " <<cgSolver.Cycle(&testgrid,&zerogrid) << std::endl;
            //std::cout << "Cycle finished" << std::endl;
            counter++;
        }
        if (key == -1) {
            run = false;
        }
    }
    
  return 0;
}
