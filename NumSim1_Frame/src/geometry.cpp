#include "geometry.hpp"

Geometry::Geometry(){
    
    _size = {0, 0};
    _length = 0;
    _h = 0;
    _velocity = {0, 0};
    _pressure = 0;
    std::cout << " Geometry constructor done " << std::endl;
    
}

  /// Loads a geometry from a file
  //auskommentiertes ist von mir programmiert letzteres aus ilias kopiert
  void Geometry::Load(const char *file){
      
      // auskommentiertes war von mir (Felix) programmierter Einlesevorgang
      
//     std::ifstream GeomFile (file);
//     std::string line;
//     std::vector<std::string> lineVector;
//     std::string sub;
//       if (GeomFile)
//       {
//           std::cout << "ParameterFile eingelesen"<< std::endl;
//           while (getline ( GeomFile, line))
//           {
//               // only for space=space in the parameter file
//                 std::istringstream iss(line);
// 
//                 iss >> sub;
//                 do{
//                     lineVector.push_back(sub);
//                     } while (iss>>sub);
// 
//                 if(lineVector[0] == "size") {
//                     _size[0] = atoi(lineVector[2].c_str());
//                     _size[1] = atoi(lineVector[3].c_str());
//                 }
//                 else if(lineVector[0] == "velocity") {
//                     _velocity[0] = atoi(lineVector[2].c_str());
//                     _velocity[1] = atoi(lineVector[3].c_str());
//                 }
//                 else if(lineVector[0] == "pressure") {
//                     _pressure = atoi(lineVector[2].c_str());
//                 }
//                 else if(lineVector[0] == "length") {
//                     _length[0] = atoi(lineVector[2].c_str());
//                     _length[1] = atoi(lineVector[3].c_str());
//                 }
//         }
//         std::cout << "Pressure = "<<_pressure<< std::endl;
//         std::cout << "Size = "<<_size[0]<<" "<< _size[1]<< std::endl;
//         std::cout << "Length = "<<_length[0]<<" "<< _length[1]<< std::endl;
//         std::cout << "Velocity = "<< _velocity[0]<<" "<< _velocity[1]<< std::endl;
//           GeomFile.close();
//       }
//       else 
//       {
//           std::cout << "Parameter File konnte nicht gelesen werden"<< std::endl;
//       }   
//   }
      
// das hier ist aus ilias kopiert
FILE* handle = fopen(file,"r");
double inval[2];
char name[20];
      
while (!feof(handle)) {
	if (!fscanf(handle, "%s =", name)) continue;
	if (strcmp(name,"size") == 0) {
		if (fscanf(handle," %lf %lf\n",&inval[0],&inval[1])) {
			_size[0] = inval[0];
			_size[1] = inval[1];
		}
		continue;
	}
	if (strcmp(name,"length") == 0) {
		if (fscanf(handle," %lf %lf\n",&inval[0],&inval[1])) {
			_length[0] = inval[0];
			_length[1] = inval[1];
		}
		continue;
	}
	if (strcmp(name,"velocity") == 0) {
		if (fscanf(handle," %lf %lf\n",&inval[0],&inval[1])) {
			_velocity[0] = inval[0];
			_velocity[1] = inval[1];
		}
		continue;
	}
	if (strcmp(name,"pressure") == 0) {
		if (fscanf(handle," %lf\n",&inval[0]))
			_pressure = inval[0];
		continue;
	}
}
fclose(handle);
//berechnet die Höhe und Breite eines Elements
_mesh[0] = _length[0]/_size[0];
_mesh[1] = _length[1]/_size[1];
}

  /// Returns the number of cells in each dimension
  const multi_index_t &Geometry::Size() const
  {
      return _size;
  }
  /// Returns the length of the domain
  const multi_real_t &Geometry::Length() const
  {
      return _length;
  }
  /// Returns the meshwidth
  const multi_real_t &Geometry::Mesh() const
  {
      return _mesh;
  }

  /// Updates the velocity field u
//   void Geometry::Update_U(Grid *u) const{
//       Iterator iter;
//       iter.First();
//       while (iter.Valid())
//       {
//           //if(iter()!=inter.First()) {u[before]=0;}
//           u[*iter()]= 0;
//           u[*iter.Next()] = 1;
//       }
//   }
//   /// Updates the velocity field v
//   void Geometry::Update_V(Grid *v) const;
//   /// Updates the pressure field p
//   void Geometry::Update_P(Grid *p) const;

