#include "geometry.hpp"

///Constructor Geometry
Geometry::Geometry(const Communicator* comm):_comm(comm)
{
    
    _size = {0, 0};
    _length = 0;
    _h = 0;
    _velocity = {0, 0};
    _pressure = 0;
    Load("default.geom");
    //std::cout << " Geometry constructor done " << std::endl;
    
}


  /// Loads a geometry from a file
void Geometry::Load(const char *file){

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
GetSizesOfThreads();
//berechnet die Höhe und Breite eines Elements

//std::cout << "Prozess " << _comm->ThreadNum() << ": Geometry constructed with _blength[0]=" << _blength[0] << "; _bsize[0]=" << _bsize[0] << "; _mesh[0]=" << _mesh[0] << std::endl;

}
    void Geometry::GetSizesOfThreads (){
        //changes the total number of cells (_size) into an even number with respect to the number of threads
        _size[0] = _size[0] - (_size[0]%(_comm->ThreadDim ()[0]*2));
        _size[1] = _size[1] - (_size[1]%(_comm->ThreadDim ()[1]*2));
        
        _mesh[0] = _length[0]/_size[0];
        _mesh[1] = _length[1]/_size[1];
        
        //calculates the number of cells for each thread (_bsize)
        _bsize[0] = _size[0]/_comm->ThreadDim ()[0];
        _bsize[1] = _size[1]/_comm->ThreadDim ()[1];
        
        if(_bsize[0] < 2 || _bsize[1] < 2){
            std::cout <<" !!! Error Inputparameter _size is too small for the number of threads used!!! " << std::endl;
        }
        
        //calculates the legnth of each thread (_blength)
        _blength[0] = _length[0]/(_comm->ThreadDim ()[0]);
        _blength[1] = _length[1]/(_comm->ThreadDim ()[1]);
        
    }
        
    
  /// Returns the number of cells in each dimension
  const multi_index_t &Geometry::Size() const
  {
      //return _size;
      return _bsize;
  }
  /// Returns the length of the domain
  const multi_real_t &Geometry::Length() const
  {
      //return _length;
      return _blength;
  }
   const multi_index_t& Geometry::TotalSize() const {
       return _size;
   }
   const multi_real_t& Geometry::TotalLength() const {
       return _length;
   }
  /// Returns the meshwidth
  const multi_real_t &Geometry::Mesh() const
  {
      return _mesh;
  }

  /// Updates the velocity field u
    //TODO Hier bin ich mir nicht sicher ob Ecke gesetzt werden müssen und wenn ja wie evtl. Iterator verändern?
    void Geometry::Update_U(Grid *u) const{
         BoundaryIterator it = BoundaryIterator(this);
        if(_comm->isLeft()){
            it.SetBoundary(4);
            it.First();
            while(it.Valid()){
                u->Cell(it) = 0;
                it.Next();
            }
        }
        if(_comm->isBottom()){
            it.SetBoundary(1);
            it.First();
            while(it.Valid()){
                u->Cell(it) = (-1.0)*u->Cell(it.Top());
                it.Next();
            }
        }
        if(_comm->isRight()){
            it.SetBoundary(2);
            it.First();
            while(it.Valid()){
                u->Cell(it) = 0;
                u->Cell(it.Left()) = 0;
                it.Next();
            }
        }
        if(_comm->isTop()){
            it.SetBoundary(3);
            it.First();
            while(it.Valid()){
                u->Cell(it) = 2.0*_velocity[0] - u->Cell(it.Down());
                it.Next();
            }
        }

//         for(int bound_num = 1; bound_num <= 4; bound_num++){
//             BoundaryIterator it = BoundaryIterator(this);
//             it.SetBoundary(bound_num);
//             it.First();
//             while(it.Valid()){
//                 switch(bound_num){
//                     case 1:
//                         u->Cell(it) = (-1.0)*u->Cell(it.Top());
//                         break;
//                     case 2:
//                         u->Cell(it) = 0;
//                         u->Cell(it.Left()) = 0;
//                         break;
//                     case 3:
//                         u->Cell(it) = 2.0*_velocity[0] - u->Cell(it.Down());
//                         break;
//                     case 4:
//                         u->Cell(it) = 0;
//                         break;
//                     default: std::cout << "Error" << std::endl;
//                 }
//                 it.Next();
//             }
//         }
        //Setzt die vier eckpunkte bzw am linken Rand auch die links davon sinnvoll 
        //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
//         BoundaryIterator it = BoundaryIterator(this);
//         it.SetBoundary(0);
//         for (index_t i = 1; i<=4; i++){
//             switch(i){
//                 case 1:
//                     u->Cell(it) = 0;
//                     break;
//                 case 2:
//                     //u->Cell(it.Left())= (-1.0)*u->Cell((it.Left()).Top());
//                     u->Cell(it) = 0;
//                     break;
//                 case 3:
//                     u->Cell(it) =  2.0*_velocity[0] - u->Cell(it.Down());
//                         break;
//                 case 4:
//                     u->Cell(it.Left()) = 2.0*_velocity[0] - u->Cell(it.Down());
//                     //u->Cell(it) = 0;
//                     break;
//                 default: std::cout << " Error bei outer 4 cells" << std::endl;
//             }
//             it.Next();
//         }
    }
   /// Updates the velocity field v
   //TODO Auch hier müssen wir uns noch überlegen wie wir die Eckpunkte behandeln
   void Geometry::Update_V(Grid *v) const{
        BoundaryIterator it = BoundaryIterator(this);
        if(_comm->isLeft()){
            it.SetBoundary(4);
            it.First();
            while(it.Valid()){
                v->Cell(it) = (-1.0)*v->Cell(it.Right());
                it.Next();
            }
        }
        if(_comm->isBottom()){
            it.SetBoundary(1);
            it.First();
            while(it.Valid()){
                v->Cell(it) = 0;
                it.Next();
            }
        }
        if(_comm->isRight()){
            it.SetBoundary(2);
            it.First();
            while(it.Valid()){
                v->Cell(it) = (-1.0)*v->Cell(it.Left());
                it.Next();
            }
        }
        if(_comm->isTop()){
            it.SetBoundary(3);
            it.First();
            while(it.Valid()){
                v->Cell(it) = _velocity[1];
                v->Cell(it.Down()) = _velocity[1];
                it.Next();
            }
        }
       
//        for(int bound_num = 1; bound_num <= 4; bound_num++){
//             BoundaryIterator it = BoundaryIterator(this);
//             it.SetBoundary(bound_num);
//             it.First();
//             while(it.Valid()){
//                 switch(bound_num){
//                     case 1:
//                         v->Cell(it) = 0;
//                         break;
//                     case 2:
//                         v->Cell(it) = (-1.0)*v->Cell(it.Left());
//                         break;
//                     case 3:
//                         v->Cell(it) = _velocity[1];
//                         v->Cell(it.Down()) = _velocity[1];
//                         break;
//                     case 4:
//                         v->Cell(it) = (-1.0)*v->Cell(it.Right());
//                         break;
//                     default: std::cout << "Error" << std::endl;
//                 }
//                 it.Next();
//             }
//         }
        //Setzt die vier eckpunkte bzw am linken Rand auch die links davon sinnvoll 
        //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
//         BoundaryIterator it = BoundaryIterator(this);
//         it.SetBoundary(0);
//         for (index_t i=1; i<=4; i++){
//             v->Cell(it)=0;
//             //Die Ecke rechts oben wird vorher falsch gesetzt
//             //deshalb hier die Korrektur
//             if(i==4){
//                 v->Cell(it.Down()) = _velocity[1];
//             }
//             it.Next();
//         }
    }
   
   /// Updates the pressure field p
   //TODO Auch hier müssen wir uns noch überlegen wie wir die Eckpunkte behandeln
   void Geometry::Update_P(Grid *p) const{
       
       BoundaryIterator it = BoundaryIterator(this);
        if(_comm->isLeft()){
            it.SetBoundary(4);
            it.First();
            while(it.Valid()){
                p->Cell(it) = p->Cell(it.Right());
                it.Next();
            }
        }
        if(_comm->isBottom()){
            it.SetBoundary(1);
            it.First();
            while(it.Valid()){
                p->Cell(it) = p->Cell(it.Top());
                it.Next();
            }
        }
        if(_comm->isRight()){
            it.SetBoundary(2);
            it.First();
            while(it.Valid()){
                p->Cell(it) = p->Cell(it.Left());
                it.Next();
            }
        }
        if(_comm->isTop()){
            it.SetBoundary(3);
            it.First();
            while(it.Valid()){
                p->Cell(it) = p->Cell(it.Down());
                it.Next();
            }
        }
       
//        for(int bound_num = 1; bound_num <= 4; bound_num++){
//             BoundaryIterator it = BoundaryIterator(this);
//             it.SetBoundary(bound_num);
//             it.First();
//             while(it.Valid()){
//                 switch(bound_num){
//                     case 1:
//                         p->Cell(it) = p->Cell(it.Top());
//                         break;
//                     case 2:
//                         p->Cell(it) = p->Cell(it.Left());
//                         break;
//                     case 3:
//                         p->Cell(it) = p->Cell(it.Down());
//                         break;
//                     case 4:
//                         p->Cell(it) = p->Cell(it.Right());
//                         break;
//                     default: std::cout << "Error" << std::endl;
//                 }
//                 it.Next();
//             }
//         }
//         //Setz die vier Eckpunkte gleich einer Nachbarzelle
//         //Hier ist die Reihenfolge so: 1: links unten, 2 :rechts unten, 3: links oben, 4: rechts oben 
//         BoundaryIterator it = BoundaryIterator(this);
//         it.SetBoundary(0);
//         for (index_t i = 1; i<=4; i++){
//             switch(i){
//                 case 1:
//                     p->Cell(it) = p->Cell(it.Right());
//                     break;
//                 case 2:
//                     p->Cell(it) = p->Cell(it.Left());
//                     break;
//                 case 3:
//                     p->Cell(it) = p->Cell(it.Down());
//                         break;
//                 case 4:
//                     p->Cell(it) = p->Cell(it.Down());
//                     break;
//             }
//             it.Next();
//         }
   }
