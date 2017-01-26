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
    
  Parameter param(paramInput);
  Geometry geom(&comm, geometryInput);
  
  multi_real_t offset = {0.5*geom.Mesh()[0],0.5*geom.Mesh()[1]};
  Grid testgrid(&geom, offset);
  
  /////////////////////////////////////////////////////
  //Test von verschiedenen Funktionen
  //
  /////////////////////////////////////////////////////
  testgrid.Initialize(0);

  bool testBoundaries = true;
  bool testRestrict = false;
  bool testInterCoarse2Fine = false;
  
Grid pFine(&geom, offset);
  pFine.Initialize(0.);
  Grid rhsFine(&geom, offset);
  rhsFine.Initialize(0.);
   Geometry coarseGeom(geom, (index_t) 1);
  Grid pCoarse(&coarseGeom);
  pCoarse.Initialize(0);
  Grid rhsCoarse(&coarseGeom);
  rhsCoarse.Initialize(0);
  Multigrid multigrid(&geom, &comm, 2);
  index_t resLevel = 0;

  if(testBoundaries){
 //multi_real_t offset = {0.5*geom.Mesh()[0],0.5*geom.Mesh()[1]};
  
  
  
  for (InteriorIterator it(&geom); it.Valid(); it.Next()){
      pFine.Cell(it) = 1.;
  }
  //pFine.Cell(it.Top()) = 1;
  //pFine.Cell(it) = 3;
  //pFine.Cell(it.Right()) = 1;
  //pFine.Cell(it.Top().Right()) = 2;
  index_t coarseLevel = 1;
 
  
  //Test für Boundaries BoundaryUpdateCoarse
  std::cout << "pFine vor Boundaries" <<std::endl;
  pFine.PrintGrid();
  std::cout << "rhsCoarse vor Boundaries" <<std::endl;
  rhsCoarse.PrintGrid();
  multigrid.Boundaries(&pFine,&rhsCoarse,(index_t)0);
  std::cout << "rhsCoarse nach Boundaries" <<std::endl;
  rhsCoarse.PrintGrid();
  std::cout << "pCoarse vor BoundaryUpdateCoarse" <<std::endl;
  pCoarse.PrintGrid();
  std::cout << "rhsCoarse vor BoundaryUpdateCoarse" <<std::endl;
  rhsCoarse.PrintGrid();
  coarseGeom.BoundaryUpdateCoarse(&pCoarse, &rhsCoarse);
  std::cout << "pCoarse vor BoundaryUpdateCoarse" <<std::endl;
  pCoarse.PrintGrid();
  
  }
  //endTest für Boundaries
  
if(testRestrict){
  resLevel = 0;
  //test für restrict
  Iterator it(&geom, (index_t) 10);
  pFine.Initialize(0.);
  rhsFine.Initialize(0.);
  pCoarse.Initialize(0.);
  rhsCoarse.Initialize(0.);
  pFine.Cell(it) = 3;
  std::cout << "pFine Before restrict" <<std::endl;
  pFine.PrintGrid();
  std::cout << "rhsCoarse Before restrict" <<std::endl;
  rhsCoarse.PrintGrid();
  multigrid.restrict(&pFine, &pCoarse, &rhsFine, &rhsCoarse, resLevel); 
  std::cout << "rhsCoarse After restrict" <<std::endl;
  rhsCoarse.PrintGrid();
  
}
if(testInterCoarse2Fine){
    Iterator it3(&geom, (index_t) 10);
  pFine.Initialize(0.);
  rhsFine.Initialize(0.);
  pCoarse.Initialize(0.);
  rhsCoarse.Initialize(0.);
  pCoarse.Cell(it3) = 3;
    std::cout << " Fine Grid before interCoarse2Fine" << std::endl;
    rhsFine.PrintGrid();
    std::cout << " Coarse Grid before interCoarse2Fine " << std::endl;
    pCoarse.PrintGrid();
    resLevel = 0;
    multigrid.interCorse2Fine(&pFine, &pCoarse, resLevel);
    std::cout << " Fine Grid after interCoarse2Fine" << std::endl;
    pFine.PrintGrid();
    std::cout << std::endl;
}
  Renderer visu(geom.Length(), geom.Mesh());
  visu.Init(400, 400);
   visu.Render(&pFine);
   //visu.Render(&pFine);
   bool run = true;
real_t residuum = 1;
index_t k = 0;
/*
while (run){
    int key = visu.Check();
    //key = (int)std::cin;
   //std::cout << " residdum = " << <<std::endl;
    if (key == 10) {
            printf("%f\n",multigrid.Cycle(&pFine, &rhsFine));
            //pFine.PrintGrid();
        }
    //residuum = multigrid.Cycle(&pFine, &rhsFine);
    visu.Render(&pFine);
    //visu.Render(&pFine);
    //std::cout << " residdum = " << residuum <<std::endl;
    k++;
    if (key == -1) run == false;
    //if (k == 5) break;
}
*/

  
  //end test restrict
 
  MPI_Barrier(MPI_COMM_WORLD);
  /////////////////////////////////////
  // Test for Multigrid functions
  /////////////////////////////////////

/*  
    // Create and initialize the visualization
    Renderer visu(geom.Length(), geom.Mesh());
    //std::cout << "Renderer done" << std::endl;
    visu.Init(700, 700);
    //std::cout << "Renderer done" << std::endl;
    const Grid *visugrid;
    visugrid=&testgrid;
    bool run = true;
    MPI_Barrier(MPI_COMM_WORLD);
    while (run) {
        // Render and check if window is closed
        int key = visu.Check();
        visu.Render(&testgrid);//, visugrid->Min(), visugrid->Max());
        if (key == 10) {
            //do RedOrBlackSOR cycle
            multigrid.Cycle(&testgrid,&zerogrid); //TODO: den Teil kann man intelligenter gestalten, indem evtl. alle auf einen Tastendruck reagieren. im Moment muss jedes Fenster einzeln gedrückt werden
        }
        if (key == -1) {
            run = false;
        }
    }
*/    
  return 0;
}
