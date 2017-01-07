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


#include "iostream"
#include "fstream"
#include <random>

int main(int argc, char **argv) {
    int i_begin = 1, i_end=10;
    if(argc>2){
        i_begin = strtol(argv[1],NULL,10);
        i_end = strtol(argv[2],NULL,10);
    }
    //Choose random distribution
    double mu = 1600.0, sigma = 1000.0/6.0;
    //Choose true random generator
    std::random_device generator;
    //Choose normal distribution
    std::normal_distribution<double> distribution(mu,sigma);
    
    //int counter=42;
    
    //double re = distribution(generator);

    /*char* command = new char[100];
    sprintf(command,"./../NumSim1_Frame/Magrathea/magrathea -o Sample%i -size 128x128",42);
    system(command);
    delete command;*/
    
    for(int i=i_begin; i<=i_end; i++){
        printf("Starting sample %i\n",i);
        //make directory for sample
        char* command1 = new char[100];
        sprintf(command1,"mkdir SampleData/Sample%i",i);
        system(command1); 
        delete command1;
        //make geometry and param files
        char* command2 = new char[200];
        sprintf(command2,"./../NumSim1_Frame/Magrathea/magrathea -o SampleData/Sample%i/default -dt 0.0018 -iter 1000 -re %f -size 128x128",i,distribution(generator));
        system(command2);
        delete command2;
        //execute simulation
        char* command3 = new char[200];
        sprintf(command3,"./../NumSim1_Frame/build/NumSim SampleData/Sample%i/default.geom SampleData/Sample%i/default.param SampleData/Sample%i/values",i,i,i);
        system(command3);
        delete command3;
    }
    
    
    return 0;
}
