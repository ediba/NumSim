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
    const Communicator comm(&argc, &argv);
    Geometry geom(&comm, geometryInput);
    geom.printGeometry();
    index_t refine = 1;
    Geometry coarse(geom,refine);
    coarse.printGeometry();
  
  return 0;
}
