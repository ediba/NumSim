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
/*    
    //---------------------------------------------------------------------
    //Program for uncertainty quantification
    const char *output;
    if(argc>3){
        output = argv[3];
    }
    double dt_dot = 5.0, t_dot=0.0;    //makes for each 5s one dot so one can see how far the program has run
    std::cout << "Progress(100%=10*): " << std::endl;
    const Communicator comm(&argc, &argv);
    Parameter param(paramInput);
    Geometry geom(&comm, geometryInput);
    Compute comp(&geom, &param, &comm);
    std::ofstream outputStream;    
    outputStream.open (output, std::ios_base::app);
    const Grid* u = comp.GetU();
    Iterator pos1(&geom,901);   //(120,5) = (120+1)+(5+1)*(128+2)
    Iterator pos2(&geom,8515);  //(64,64) = (64+1)+(64+1)*(128+2)
    Iterator pos3(&geom,15736); //(5,120) = (5+1)+(120+1)*(128+2)
    while (comp.GetTime() < param.Tend()){
        outputStream << u->Cell(pos1) << "\t" << u->Cell(pos2) << "\t" << u->Cell(pos3) << "\n" ;
        comp.TimeStep(false);
        if(comp.GetTime() > t_dot){
            std::cout << "*"<<std::endl;
            t_dot+=dt_dot;
        }
    }
    std::cout << std::endl;
    outputStream << u->Cell(pos1) << "\t" << u->Cell(pos2) << "\t" << u->Cell(pos3) << "\n" ;
    outputStream.close();
    //-----------------------------------------------------------------------
*/    
    bool visualize = true;
    int visuStep = 5;
    bool writeVtk = true;
    
  // Create parameter and geometry instances with default values
  
    //Communicator:
  const Communicator comm(&argc, &argv);
//   if(comm.ThreadNum()==0) 
      std::cout << "main start with "<<comm.ThreadCnt() <<" Threads"<< std::endl;
    
  Parameter param(paramInput);
  Geometry geom(&comm, geometryInput);
  // Create the fluid solver
  Compute comp(&geom, &param, &comm);
  // Create and initialize the visualization
    Renderer visu(geom.Length(), geom.Mesh());
    if(visualize){ 
        int xsize=1000;
        int ysize=xsize*(int)geom.TotalLength()[1]/geom.TotalLength()[0];
        visu.Init(xsize/comm.ThreadDim()[0], ysize/comm.ThreadDim()[1]);
    }


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
visugrid = comp.GetVelocity();
int k = 0;
if(comm.ThreadNum() == 0){
    zg.Start();
}
    while (comp.GetTime() < param.Tend() && run) {
        if(writeVtk && comp.GetTime() >= t_nextVtk){
            vtk.Init("VTK/field");
            vtk.AddCellScalar("p_Cell",comp.GetP());
            vtk.AddCellScalar("v",comp.GetV());
            vtk.AddCellScalar("u",comp.GetU());
            vtk.AddCellScalar("Velocity",comp.GetVelocity());
            vtk.AddCellScalar("Vorticity",comp.GetVorticity());
            vtk.AddCellScalar("Streamlines",comp.GetStream());
            vtk.SwitchToPointData();
            vtk.AddPointScalar("p_Cell",comp.GetP());
            vtk.AddPointScalar("v",comp.GetV());
            vtk.AddPointScalar("u",comp.GetU());
            vtk.AddPointScalar("Velocity",comp.GetVelocity());
            vtk.AddPointScalar("Vorticity",comp.GetVorticity());
            vtk.AddPointScalar("Streamlines_point",comp.GetStream());
            vtk.Finish();
            t_nextVtk += dt_vtk;
        }
        //visugrid = comp.GetVelocity();
        //Render and check if window is closed
        //int key = visu.Check();
        
        if(visualize && k%visuStep == 0){
            switch (visu.Render(visugrid, visugrid->Min(), visugrid->Max())){
                case -1:
                    run = false;
                    break;
                case 0: 
                    visugrid = comp.GetVelocity();
                    break;
                case 1:
                    visugrid = comp.GetU();
                    break;
                case 2:
                    visugrid = comp.GetV();
                    break;
                case 3:
                    visugrid = comp.GetP();
                    break;
                case 4:
                    visugrid = comp.GetVorticity();
                    break;
                case 5:
                    visugrid = comp.GetStream();
                    break;
                case 6:
                    visugrid = comp._particleTracing;
                    break;
                case 7:
                    visugrid = comp._streakLines;
                    break;
                default:
                    break;
            }
        }//, visugrid->Min(), visugrid->Max());
        //if (key == 10) {
            //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
            comp.TimeStep(true);
            k++;
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
