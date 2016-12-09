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

    int xsize=800;
    int ysize=800;
    //visu.Init(xsize/comm.ThreadDim()[0], ysize/comm.ThreadDim()[1]);


    //Delete old files
    if(comm.ThreadNum() == 0) {
        system("exec rm -r ./VTK/*");
    }
  // Create a VTK generator
  //VTK vtk(geom.Mesh(), geom.Size());
    real_t dt_vtk = 1.0, t_nextVtk=0.3;
    multi_real_t offset_vtk = {comm.ThreadIdx()[0]*(geom.Length()[0]),comm.ThreadIdx()[1]*(geom.Length()[1])};
    multi_index_t local_size = geom.Size();
    local_size[0]+=2;
    local_size[1]+=2;
    multi_index_t global_size = geom.TotalSize();
    global_size[0]+=2;
    global_size[1]+=2;
    VTK vtk(geom.Mesh(), local_size,global_size,offset_vtk,comm.ThreadNum(), comm.ThreadCnt(), comm.ThreadDim());

  const Grid *visugrid;
  bool run = true;
ZeitGeist zg;
MPI_Barrier(MPI_COMM_WORLD);
if(comm.ThreadNum() == 0){
    zg.Start();
}
    while (comp.GetTime() < 50 && run) {
//         if(comp.GetTime() >= t_nextVtk){
//             vtk.Init("VTK/field");
//             vtk.AddCellScalar("p",comp.GetP());
//             vtk.AddCellScalar("v",comp.GetV());
//             vtk.AddCellScalar("u",comp.GetU());
//             vtk.AddCellScalar("Velocity",comp.GetVelocity());
//             vtk.AddCellScalar("Vorticity",comp.GetVorticity());
//             vtk.SwitchToPointData();
//             vtk.Finish();
//             t_nextVtk += dt_vtk;
//         }
        //visugrid = comp.GetVelocity();
        // Render and check if window is closed
        //int key = visu.Check();
        //visu.Render(visugrid,0.0,1.0);//, visugrid->Min(), visugrid->Max());
        //if (key == 10) {
            //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
            comp.TimeStep(false);
        //}
//         if (key == -1) {
//             run = false;
//         }
    }
std::ofstream myfile;
if(comm.ThreadNum() == 0){
    std::cout << "Zeit gebraucht : " <<zg.Stop()/CLOCKS_PER_SEC<<std::endl;
    
  myfile.open ("Zeitmessung.txt", std::ios_base::app);
  myfile << comm.ThreadCnt() << " " << zg.Stop()/CLOCKS_PER_SEC <<"\n" ;
  myfile.close();
}

    
  return 0;
}
