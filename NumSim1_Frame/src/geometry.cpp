#include "geometry.hpp"

Geometry::Geometry(){
    
    _size = {0, 0};
    _length = 0;
    _h = 0;
    _velocity = {0, 0};
    _pressure = 0;
    Load("default.geom");
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
    void Geometry::Update_U(Grid *u) const{
        //std::cout << " Velocity = " << _velocity[0] << " " << _velocity[1]<< std::endl;
        for(int bound_num = 1; bound_num <= 4; bound_num++){
            BoundaryIterator it = BoundaryIterator(this);
            it.SetBoundary(bound_num);
            it.First();
            while(it.Valid()){
                switch(bound_num){
                    case 1:
                        u->Cell(it) = (-1)*u->Cell(it.Top());
                        //std::cout << "case " << bound_num <<" Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                        break;
                    case 2:
                        u->Cell(it) = 0;
                        u->Cell(it.Left()) = 0;
                        //std::cout << "case " << bound_num <<" Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                        //TODO:Was ist mit den Zellen rechts daneben? Wichtig?
                        break;
                    case 3:
                        u->Cell(it) = 2.0*_velocity[0] - u->Cell(it.Down());
                        //std::cout << "2* " << _velocity[0] << " - " <<  u->Cell(it.Down())<< " it.Down() = "<< it.Down()<< std::endl;
                        //std::cout << "case " << bound_num <<" Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                        break;
                    case 4:
                        u->Cell(it) = 0;
                        //std::cout << "case " << bound_num <<" Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                        break;
                    default: std::cout << "Error" << std::endl;
                }
                it.Next();
            }
        }
        //Setzt die vier eckpunkte bzw am linken Rand auch die links davon sinnvoll 
        //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
        BoundaryIterator it = BoundaryIterator(this);
        it.SetBoundary(0);
        for (index_t i = 1; i<=4; i++){
            switch(i){
                case 1:
                    u->Cell(it) = 0;
                   //std::cout << "case 0 Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                    break;
                case 2:
                    u->Cell(it.Left())= (-1.0)*u->Cell((it.Left()).Top());
                    u->Cell(it) = 0;
                    //std::cout << "case  0  Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                    break;
                case 3:
                    //hier ists egal
                    u->Cell(it) = 0;
                    //u->Cell(it.Left()) = 0;
                    //std::cout << "case  0  Cell Nr: "<<it.Left()<< " set to: " << u->Cell(it)<<std::endl;
                        break;
                case 4:
                    u->Cell(it.Left()) = 0;
                    u->Cell(it) = 0;
                    //std::cout << "case  0  Cell Nr: "<<it.Value()<< " set to: " << u->Cell(it)<<std::endl;
                    break;
                default: std::cout << " Error bei outer 4 cells" << std::endl;
            }
            it.Next();
        }
        
        //TODO:Ecken machen irgendwie noch nicht wirklich Sinn, sind im Moment auf irgendwelchen Werten
    }
   /// Updates the velocity field v
   void Geometry::Update_V(Grid *v) const{
       for(int bound_num = 1; bound_num <= 4; bound_num++){
            BoundaryIterator it = BoundaryIterator(this);
            it.SetBoundary(bound_num);
            it.First();
            while(it.Valid()){
                switch(bound_num){
                    case 1:
                        v->Cell(it) = 0;
                        break;
                    case 2:
                        v->Cell(it) = (-1.0)*v->Cell(it.Left());
                        break;
                    case 3:
                        //v->Cell(it.Down()) = 0;
                        //TODO: Was ist mit den Zellen darüber? Wichtig?
                        //Und die Zellen drüber würd ich auch auf den gleichen Wert setzten
                        // Ich denke hier ist der zweite Wert von _velocity 
                        v->Cell(it) = _velocity[1];
                        v->Cell(it.Down()) = _velocity[1];
                        break;
                    case 4:
                        v->Cell(it) = (-1.0)*v->Cell(it.Right());
                        break;
                    default: std::cout << "Error" << std::endl;
                }
                it.Next();
            }
        }

        //Setzt die vier eckpunkte bzw am linken Rand auch die links davon sinnvoll 
        //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
        BoundaryIterator it = BoundaryIterator(this);
        it.SetBoundary(0);
        for (index_t i=1; i<=4; i++){
            v->Cell(it)=0;
            //Die Ecke rechts oben wird vorher falsch gesetzt
            //deshalb hier die Korrektur
            if(i==4){
                v->Cell(it.Down()) = _velocity[1];
            }
            it.Next();
            //std::cout<< " Cell number " << it << " set to 0" <<std::endl;
        }
    }
        //TODO:Ecken machen irgendwie noch nicht wirklich Sinn, sind im Moment auf irgendwelchen Werten
   
   /// Updates the pressure field p
   void Geometry::Update_P(Grid *p) const{
       //TODO:Im Moment zero Gradient. Noch nicht kontrolliert ob das richtig ist
       for(int bound_num = 1; bound_num <= 4; bound_num++){
            BoundaryIterator it = BoundaryIterator(this);
            it.SetBoundary(bound_num);
            it.First();
            while(it.Valid()){
                switch(bound_num){
                    case 1:
                        p->Cell(it) = p->Cell(it.Top());
                        break;
                    case 2:
                        p->Cell(it) = p->Cell(it.Left());
                        break;
                    case 3:
                        p->Cell(it) = p->Cell(it.Down());
                        break;
                    case 4:
                        p->Cell(it) = p->Cell(it.Right());
                        break;
                    default: std::cout << "Error" << std::endl;
                }
                it.Next();
            }
        }
        //Setz die vier Eckpunkte gleich einer Nachbarzelle
        //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
        BoundaryIterator it = BoundaryIterator(this);
        it.SetBoundary(0);
        for (index_t i = 1; i<=4; i++){
            switch(i){
                case 1:
                    p->Cell(it) = p->Cell(it.Right());
                    break;
                case 2:
                    p->Cell(it) = p->Cell(it.Left());
                    break;
                case 3:
                    p->Cell(it) = p->Cell(it.Down());
                        break;
                case 4:
                    p->Cell(it) = p->Cell(it.Down());
                    break;
            }
            it.Next();
        }
   }
