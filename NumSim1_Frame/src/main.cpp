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
  Parameter param;
  Geometry geom(&comm);
  // Create the fluid solver
  std::cout << "compute start" << std::endl;
  Compute comp(&geom, &param);
  std::cout << "compute done" << std::endl;

  // Create and initialize the visualization
  Renderer visu(geom.Length(), geom.Mesh());
    std::cout << "Renderer done" << std::endl;
  visu.Init(800, 800);
    std::cout << "Renderer done" << std::endl;

  // Create a VTK generator
  VTK vtk(geom.Mesh(), geom.Size());

  const Grid *visugrid;
  bool run = true;

  //Ask user what to run
  bool whileloop=true;
  while(whileloop){
    std::cout << "Which program would u like to run?" << std::endl;
    std::cout << "1:\tDriven Cavity (not working properly)" <<std::endl;
    std::cout << "2:\tIteratorTest"<<std::endl;
    std::cout << "3:\tUpdate Boundary Condition Test"<<std::endl;
    std::cout << "4:\tSOR Test"<<std::endl;
    std::cout << "0:\tEscape"<<std::endl;
    char choice;
    std::cin>>choice;
    switch(choice){
        case '1':{
            while (comp.GetTime() < 50 && run) {
              visugrid = comp.GetVelocity();
                // Render and check if window is closed
                int key = visu.Check();
                visu.Render(visugrid);//, visugrid->Min(), visugrid->Max());
                if (key == 10) {
                    //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                    comp.TimeStep(true);
                }
                if (key == -1) {
                    run = false;
                }
            }
        }
            break;
        case '2':{
            bool whileloop2=true;
            while(whileloop2){
                std::cout << "Which program would u like to run?" << std::endl;
                std::cout << "1:\tIterator" <<std::endl;
                std::cout << "2:\tInteriorIterator"<<std::endl;
                std::cout << "3:\tBoundary1"<<std::endl;
                std::cout << "4:\tBoundary2"<<std::endl;
                std::cout << "5:\tBoundary3"<<std::endl;
                std::cout << "6:\tBoundary4"<<std::endl;
                std::cout << "7:\tBoundary0"<<std::endl;
                std::cout << "0:\tEscape"<<std::endl;
                char choice2;
                std::cin>>choice2;
                switch(choice2){
                    case '1':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        Iterator it(&geom);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    break;
                    case '2':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        InteriorIterator it(&geom);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    break;
                    case '3':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        BoundaryIterator it(&geom);
                        it.SetBoundary(1);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    break;
                    case '4':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        BoundaryIterator it(&geom);
                        it.SetBoundary(2);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    break;
                    case '5':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        BoundaryIterator it(&geom);
                        it.SetBoundary(3);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    break;
                    case '6':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        BoundaryIterator it(&geom);
                        it.SetBoundary(4);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    case '7':{
                        bool plot=true;
                        Grid testgrid(&geom,{geom.Mesh()[0]*0.5,geom.Mesh()[1]*0.5});
                        visugrid=&testgrid;
                        testgrid.Initialize(0);
                        BoundaryIterator it(&geom);
                        it.SetBoundary(0);
                        testgrid.Cell(it)=1;
                        while (plot) {
                            // Render and check if window is closed
                            int key = visu.Check();
                            visu.Render(visugrid, visugrid->Min(), visugrid->Max());
                            if (key == 10) {
                                //printf("%f\n",sor.Cycle(&testgrid5,&zeroGrid));
                                testgrid.Cell(it)=0;
                                it.Next();
                                testgrid.Cell(it)=1;
                            }
                            if (key == -1) {
                                plot = false;
                            }
                        }
                    }
                    break;
                    case '0':
                        whileloop2=false;
                        break;
                }
            }
        }
            break;
        case '3':
            std::cout<<"not enough time to write down"<<std::endl;
            break;
        case '4':{
            //unit test SOR solver
            multi_real_t offset = {0.5*geom.Mesh()[0],0.5*geom.Mesh()[1]};
            Grid testgrid5(&geom, offset);
            testgrid5.Initialize(0);
            Grid zeroGrid(&geom,offset);
            zeroGrid.Initialize(0);
            Iterator it5(&geom, (geom.Size()[0]+2)*(geom.Size()[1]+2)/2+(geom.Size()[0]/2));
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
            }}
            break;
        case '0':
            whileloop=false;
            break;
            
    }
  }
  return 0;
}
