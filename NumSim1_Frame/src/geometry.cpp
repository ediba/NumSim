#include "geometry.hpp"

///Constructor Geometry
Geometry::Geometry(const Communicator* comm):_comm(comm)
{
    
    _size = {0, 0};
    _length = 0;
    _h = 0;
    _velocity = {0, 0};
    _pressure = 0;
    _freeGeom = false;
    //_flag= new char [_size[0]*_size[1]];
    Load("UpdateTest.geom");
    //std::cout << " Geometry constructor done " << std::endl;
    
}


  /// Loads a geometry from a file
void Geometry::Load(const char *file){

FILE* handle = fopen(file,"r");
double inval[2];
char name[20];
bool GameOVER=false;
std::ofstream myfile;
myfile.open ("example.txt");

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
	if (strcmp(name,"geometry") == 0) {
            if (fscanf(handle," %s\n",&name)){
                    if (strcmp(name,"free")==0){
                        _freeGeom = true;
                        
                        //std::cout << "free geom " << _freeGeom <<std::endl;
                    }
                    //continue;
                }
                
	}
	if(_freeGeom){
            std::cout << " Read Free geom start" << std::endl;
            std::cout <<"Size[0] = "<< _size[0] <<" Size[1] = " << _size[1] <<std::endl;
            _flag= new char [_size[0]*_size[1]];
            for (int col = _size[1]-1; col>=0 ; col--){
                for (int row = 0; row < _size[0]; row++){
                    int getThatChar=fgetc(handle);
                    _flag[col*_size[0]+row] = (char) getThatChar;
                    std::cout << "Stelle: " << col*_size[0]+row<< " = "<< _flag[col*_size[0]+row]<<std::endl;
                }
                int getThatChar=fgetc(handle);
            }
//             for(int i = 0; i<_size[0]*_size[1]; i++){
//                 int getThatChar=fgetc(handle);
//                 if((i+1)%_size[0] == 0){
//                     int getThatChar=fgetc(handle);
//                     //j --;
//                 }
//                 _flag[j*_size[0]-1+i] = (char) getThatChar;
//                 //_flag[i] = (char) getThatChar;
//                 
//             }
//             break;
        }
}
for (int i=0;i <_size[0]*_size[1]; i++){
                myfile << _flag[i];
                if((i+1)%_size[0]==0){myfile<<std::endl;}
}
myfile.close();
fclose(handle);
//Correction for using the right size 
if(_freeGeom){
    _size[0] = _size[0] -2;
    _size[1] = _size[1] -2;
}
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
    }
    /// Updates the velocity field v
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
    }
    /// Updates the pressure field p
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
   }
    bool Geometry::FreeGeometry(){return _freeGeom;}
    
    ///Updates free geometry
    void Geometry::Update_U_free(Grid *u){
        Iterator it(this);
        while (it.Valid()){
            switch(Flag(it)){
                //Water
                case ' ':
                    break;
                //NoSlip Boundary
                case '#':
                    Set_U_noslip(it, u);
                    break;
                //Vertical Inflow
                case 'V':
                    Set_U_veritcal(it, u);
                    break;
                //Horizontal Inflow
                case 'H':
                    Set_U_horizontal(it, u);
                    break;
                //Slip Boundary in vertical direction
                case '|':
                    Set_U_slipV(it, u);
                    break;
                //Slip Boundary in horizontal direction
                case '-':
                    Set_U_slipH(it, u);
                    break;
                //Outflow Boundary
                case 'O':
                    Set_U_outflow(it, u);
                    break;
                //Inflow Boundary
                case 'I':
                    //Müsste für u das gleiche sein wie case H 
                    Set_U_horizontal(it, u);
                    break;
                default: 
                    std::cout << "Not a declared Variable in the flag array"<<std::endl;
                    break;
            }
            it.Next();
        }
    }
    //Setzt die Boundary für das u grid bei vertikaler Boundary
    //Geht davon aus dass nur ein Fluid Nachbar existiert
    void Geometry::Set_U_veritcal(Iterator it, Grid* u){
        if(Flag(it.Top()) == ' '){
            u->Cell(it) = -u->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            u->Cell(it) = 0;
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it.Left()) = 0;
            u->Cell(it) = 0;
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it) = - u->Cell(it.Down());
        }
        else {
            std::cout << "Set_U_vertical Problem" << std::endl;
        }
    }
    void Geometry::Set_U_horizontal(Iterator it, Grid* u){    
        if(Flag(it.Top()) == ' '){
            u->Cell(it) = 2.0*_velocity[0] - u->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            u->Cell(it) = _velocity[0];
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it.Left()) = _velocity[0];
            u->Cell(it) = _velocity[0];
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it) = 2.0*_velocity[0] - u->Cell(it.Down());
        }
        else {
            std::cout << "Set_U_horizontal Problem" << std::endl;
        }
    }
    //TODO muss noch geändert werden wenn auch obstacles mit dabei sind
    void Geometry::Set_U_noslip(Iterator it, Grid* u){
        if(Flag(it.Top()) == ' '){
            u->Cell(it) = -1.0*u->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            u->Cell(it) = 0;
            u->Cell(it.Left()) = 0;
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it.Left())=0;
            u->Cell(it)=0;
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it)= -1.0*u->Cell(it.Down());
        }
        else {
            u->Cell(it) = 0;
            std::cout << "Set_U_noslip cornerpoint at it: "<< it << std::endl;
        }
    }
    //TODO bin mir nicht sicher für top und bottom
    void Geometry::Set_U_outflow(Iterator it, Grid *u){
        if(Flag(it.Top()) == ' '){
            u->Cell(it) = u->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            u->Cell(it) = u->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it.Left()) = u->Cell(it.Left().Left());
            u->Cell(it) = u->Cell(it.Left().Left());
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it)= u->Cell(it.Down());
        }
        else {
            std::cout << "Set_U_outflow Problem" << std::endl;
        }
    }
    void Geometry::Set_U_slipV(Iterator it, Grid* u){
        if(Flag(it.Right()) == ' '){
            u->Cell(it) = 0;
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it.Left()) = 0;
            u->Cell(it) = 0;
        }
        else {
            std::cout << "Set_U_slipV Problem" << std::endl;
        }
    }
    void Geometry::Set_U_slipH(Iterator it, Grid *u){
        if(Flag(it.Top()) == ' '){
            u->Cell(it) = u->Cell(it.Top());
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it) = u->Cell(it.Down());
        }
        else {
            std::cout << "Set_U_slipH Problem" << std::endl;
        }
        
    }
    
    
    //     void Geometry::Set_U_slip(Iterator it, Grid* u){
//     }
//     void Geometry::Update_U_free(Grid *u){
//         BoundaryIterator it;
//         it.SetBoundary(4);
//         while (it.Valid()){
//             ///NoSlip
//             if(Flag(it) == "#"){
//                 if(Flag(it.Top() == ' '){
//                     u->Cell(it) = (-1.0)*u->Cell(it.Top());
//                 }
//                 }
//             }
//             it.Next();
//         }
//         it.SetBoundary(1){
//             if(Flag(it) == "#"){
//             }
//         }
//     }
char &Geometry::Flag(const Iterator &it){
  return _flag[it.Value()];
}


const char &Geometry::Flag(const Iterator &it) const{
  return _flag[it.Value()];
}