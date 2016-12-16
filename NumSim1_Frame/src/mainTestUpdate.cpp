#include "typedef.hpp"
#include "compute.hpp"
#include "geometry.hpp"
#include "parameter.hpp"
#include "grid.hpp"
#include "visu.hpp"
#include "visu.cpp"
#include "vtk.hpp"
#include "solver.hpp"
#include "zeitgeist.hpp"
#include "iostream"
#include "fstream"

int main(int argc, char **argv) {
  // Create parameter and geometry instances with default values
  
    //Communicator:
  const Communicator comm(&argc, &argv);
//   if(comm.ThreadNum()==0) 
      std::cout << "main start with "<<comm.ThreadCnt() <<" Threads"<< std::endl;
    
  Parameter param;
  Geometry geom(&comm);
  // Create the fluid solver
  Compute comp(&geom, &param, &comm);
  // Create and initialize the visualization
  //Renderer visu(geom.Length(), geom.Mesh());
  Grid TestGrid(&geom);
  
  TestGrid.Initialize(3);
  geom.Update_U_free(&TestGrid);
  //geom.Update_U_free(&TestGrid);
    TestGrid.PrintGrid();
// for(Iterator i(&geom); i < 10; i.Next()){
//     if (geom.Flag(i.Right()) == ' '){
//         std::cout << " Leerzeicen bei i = " << i <<std::endl;
//     }
// }
  return 0;
}
