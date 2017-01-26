#include "geometry.hpp"

///Constructor Geometry
Geometry::Geometry(const Communicator* comm, const char*geometryInput):_comm(comm)
{
    
    _size = {0, 0};
    _length = 0;
    _h = 0;
    _velocity = {0, 0};
    _pressure = 0;
    _freeGeom = false;
    _numInteriorBounds = 0;
    //_flag= new char [_size[0]*_size[1]];
    Load(geometryInput);
    //std::cout << " Geometry constructor done " << std::endl;
}

Geometry::Geometry(const Geometry &other, index_t coarseLevel)
    :_size(other._size), _length(other._length), _h(other._h), _velocity(other._velocity), _pressure(other._pressure), _freeGeom(other._freeGeom), _numInteriorBounds(other._numInteriorBounds), _mesh(other._mesh), _bsize(other._bsize), _comm(other._comm), _blength(other._blength)
{
    _size[0]=_size[0]/(1<<coarseLevel);
    _size[1]=_size[1]/(1<<coarseLevel);
    _bsize[0]=_bsize[0]/(1<<coarseLevel);
    _bsize[1]=_bsize[1]/(1<<coarseLevel);
    //_h = _h*(1<<coarseLevel);
    _mesh[0] = _mesh[0]*(1<<coarseLevel);
    _mesh[1] = _mesh[1]*(1<<coarseLevel);
}
    

  /// Loads a geometry from a file
void Geometry::Load(const char *file){

FILE* handle = fopen(file,"r");
double inval[2];
char name[20];
bool GameOVER=false;
//std::ofstream myfile;
//myfile.open ("example.txt");

bool geometryRead = false;

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
	if(_freeGeom & !geometryRead){
            //std::cout << " Read Free geom start" << std::endl;
            //std::cout <<"Size[0] = "<< _size[0] <<" Size[1] = " << _size[1] <<std::endl;
            _flag= new char [_size[0]*_size[1]];
            for (int col = _size[1]-1; col>=0 ; col--){
                for (int row = 0; row < _size[0]; row++){
                    int getThatChar=fgetc(handle);
                    _flag[col*_size[0]+row] = (char) getThatChar;
                    //std::cout << "Stelle: " << col*_size[0]+row<< " = "<< _flag[col*_size[0]+row]<<std::endl;
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
            geometryRead = true;
        }
}
/*for (int i=0;i <_size[0]*_size[1]; i++){
                myfile << _flag[i];
                if((i+1)%_size[0]==0){myfile<<std::endl;}
}
myfile.close();
fclose(handle);*/
//Correction for using the right size 
if(_freeGeom){
    _size[0] = _size[0] -2;
    _size[1] = _size[1] -2;
}
GetSizesOfThreads();
if(_freeGeom){
    InteriorIterator it(this);
    while(it.Valid()){
        if(Flag(it) != ' '){
            _numInteriorBounds++;
        }
        it.Next();
    }
}
//berechnet die Höhe und Breite eines Elements

std::cout << "Prozess " << _comm->ThreadNum() << ": Geometry constructed with _blength[0]=" << _blength[0] << "; _bsize[0]=" << _bsize[0] << "; _mesh[0]=" << _mesh[0] << std::endl;

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
        if(_freeGeom){
            Update_U_free(u);
        }
        else{
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
    }
    /// Updates the velocity field v
    void Geometry::Update_V(Grid *v) const{
        if(_freeGeom){
            Update_V_free(v);
        }
        else{
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
    }
    /// Updates the pressure field p
    void Geometry::Update_P(Grid *p) const{
        if(_freeGeom){
            Update_P_free(p);
        }
        else{
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
   }
    bool Geometry::FreeGeometry(){
        return _freeGeom;
    }
    const index_t Geometry::NumInteriorBounds() const{
        return _numInteriorBounds;
    }
    
    ///Updates free geometry
    void Geometry::Update_U_free(Grid *u)const{
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
                    Set_U_vertical(it, u);
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
                    Set_U_inflow(it, u);
                    break;
                default: 
                    std::cout << "Not a declared Variable in the flag array U"<<std::endl;
                    break;
            }
            it.Next();
        }
    }
    void Geometry::Update_V_free(Grid *v)const{
        Iterator it(this);
        while (it.Valid()){
            switch(Flag(it)){
                    //Water
                    case ' ':
                        break;
                    //NoSlip Boundary
                    case '#':
                        Set_V_noslip(it, v);
                        break;
                    //Vertical Inflow
                    case 'V':
                        Set_V_vertical(it, v);
                        break;
                    //Horizontal Inflow
                    case 'H':
                        Set_V_horizontal(it, v);
                        break;
                    //Slip Boundary in vertical direction
                    case '|':
                        Set_V_slipV(it, v);
                        break;
                    //Slip Boundary in horizontal direction
                    case '-':
                        Set_V_slipH(it, v);
                        break;
                    //Outflow Boundary
                    case 'O':
                        Set_V_outflow(it, v);
                        break;
                    //Inflow Boundary
                    case 'I':
                        Set_V_inflow(it, v);
                        break;
                    default: 
                        std::cout << "Not a declared Variable in the flag array V"<<std::endl;
                        break;
                }
                it.Next();
            }
    }
    void Geometry::Update_P_free(Grid *p)const{
        Iterator it(this);
        while (it.Valid()){
            switch(Flag(it)){
                    //Water
                    case ' ':
                        break;
                    //NoSlip Boundary
                    case '#':
                        Set_P_noslip(it, p);
                        break;
                    //Vertical Inflow
                    case 'V':
                        Set_P_vertical(it, p);
                        break;
                    //Horizontal Inflow
                    case 'H':
                        Set_P_horizontal(it, p);
                        break;
                    //Slip Boundary in vertical direction
                    case '|':
                        Set_P_slipV(it, p);
                        break;
                    //Slip Boundary in horizontal direction
                    case '-':
                        Set_P_slipH(it, p);
                        break;
                    //Outflow Boundary
                    case 'O':
                        Set_P_outflow(it, p);
                        break;
                    //Inflow Boundary
                    case 'I':
                        Set_P_inflow(it, p);
                        break;
                    default: 
                        std::cout << "Not a declared Variable in the flag array P"<<std::endl;
                        break;
                }
                it.Next();
            }
    }
    //Setzt die Boundary für das u grid bei vertikaler Boundary
    //Geht davon aus dass nur ein Fluid Nachbar existiert
    void Geometry::Set_U_vertical(Iterator it, Grid* u)const{
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
           // std::cout << "Set_U_vertical Problem" << std::endl;
        }
    }
    void Geometry::Set_U_horizontal(Iterator it, Grid* u)const{
        real_t ycoord = Mesh()[1]*it.Pos()[1] - 0.5*Mesh()[1];
        if(Flag(it.Right()) == ' '){
            u->Cell(it) = -4.0*ycoord*ycoord * (_velocity[0]/(_blength[1]*_blength[1])) + 4* ycoord *(_velocity[0]/_blength[1]);
            u->Cell(it.Left()) = -4.0*ycoord*ycoord * (_velocity[0]/(_blength[1]*_blength[1])) + 4* ycoord *(_velocity[0]/_blength[1]);
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it) = -4.0*ycoord*ycoord * (_velocity[0]/(_blength[1]*_blength[1])) + 4* ycoord *(_velocity[0]/_blength[1]);
        }
        else if(Flag(it.Top())== ' '){
            std::cout << "V Vertical Boundary sollte so nicht auftauchen " << std::endl;
        }
        else if(Flag(it.Down()) == ' '){
            std::cout << "V Vertical Boundary sollte so nicht auftauchen " << std::endl;
        }
        else {
           // std::cout << "Set_U_vertical Problem" << std::endl;
        }
    }
    void Geometry::Set_U_inflow(Iterator it, Grid* u)const{    
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
           // std::cout << "Set_U_horizontal Problem" << std::endl;
        }
    }
    void Geometry::Set_U_noslip(Iterator it, Grid* u)const{
        if(Flag(it.Right()) == ' '){
            u->Cell(it) = 0;
            //u->Cell(it.Left()) = 0;
        }
        else if(Flag(it.Left()) == ' '){
            if(Flag(it.Down()) == ' '){
                u->Cell(it.Left())=0;
                u->Cell(it)=-u->Cell(it.Down());
            }
            else if(Flag(it.Top()) == ' '){
                u->Cell(it.Left())=0;
                u->Cell(it)=-u->Cell(it.Top());
            }
            else{
                u->Cell(it.Left())=0;
                u->Cell(it)=0;
            }
        }
        else if(Flag(it.Top()) == ' '){
            u->Cell(it) = -1.0*u->Cell(it.Top());
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it)= -1.0*u->Cell(it.Down());
        }
        else {
            u->Cell(it) = 0;
           // std::cout << "Set_U_noslip cornerpoint at it: "<< it << std::endl;
        }
    }
    void Geometry::Set_U_outflow(Iterator it, Grid *u)const{
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
           // std::cout << "Set_U_outflow Problem" << std::endl;
        }
    }
    void Geometry::Set_U_slipV(Iterator it, Grid* u)const{
        if(Flag(it.Top()) == ' '){
            u->Cell(it) = 0;
        }
        else if (Flag(it.Down()) == ' '){
            u->Cell(it) = 0;
        }
        else if(Flag(it.Right()) == ' '){
            u->Cell(it) = 0;
        }
        else if(Flag(it.Left()) == ' '){
            u->Cell(it.Left()) = 0;
            u->Cell(it) = 0;
        }
        else {
           // std::cout << "Set_U_slipV Problem" << std::endl;
        }
    }
    void Geometry::Set_U_slipH(Iterator it, Grid *u)const{
        if(Flag(it.Left()) == ' '){
            u->Cell(it) = u->Cell(it.Left().Left());
            u->Cell(it.Left()) = u->Cell(it.Left().Left());
        }
        if(Flag(it.Right()) == ' '){
            u->Cell(it) = u->Cell(it.Right());
        }
        else if(Flag(it.Top()) == ' '){
            u->Cell(it) = u->Cell(it.Top());
        }
        else if(Flag(it.Down()) == ' '){
            u->Cell(it) = u->Cell(it.Down());
        }
        else {
           // std::cout << "Set_U_slipH Problem" << std::endl;
        }
        
    }
    
    /////////////////////////////////////////////
    //V functions
    /////////////////////////////
    void Geometry::Set_V_inflow(Iterator it, Grid* v)const{
        if(Flag(it.Top()) == ' '){
            v->Cell(it) = _velocity[1];
        }
        else if(Flag(it.Right()) == ' '){
            v->Cell(it) = 2*_velocity[1] - v->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            v->Cell(it) = 2*_velocity[1] - v->Cell(it.Left());
        }
        else if(Flag(it.Down()) == ' '){
            v->Cell(it) = _velocity[1];
            v->Cell(it.Down()) = _velocity[1];
        }
        else {
         //   std::cout << "Set_V_vertical Problem" << std::endl;
        }
    }
    void Geometry::Set_V_horizontal(Iterator it, Grid* v)const{    
        if(Flag(it.Top()) == ' '){
            v->Cell(it) = 0;
        }
        else if(Flag(it.Right()) == ' '){
            v->Cell(it) = - v->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            v->Cell(it) = - v->Cell(it.Left());
        }
        else if(Flag(it.Down()) == ' '){
            v->Cell(it) = 0;
            v->Cell(it.Down()) = 0;
        }
        else {
         //   std::cout << "Set_V_horizontal Problem" << std::endl;
        }
    }

    void Geometry::Set_V_vertical(Iterator it, Grid* v)const{
        real_t xcoord = Mesh()[0]*it.Pos()[0] - 0.5*Mesh()[0];
        if(Flag(it.Top()) == ' '){
            v->Cell(it) = -4.0*xcoord*xcoord * (_velocity[1]/(_blength[0]*_blength[0])) + 4* xcoord *(_velocity[1]/_blength[0]);
            v->Cell(it.Down()) = -4.0*xcoord*xcoord * (_velocity[1]/(_blength[0]*_blength[0])) + 4* xcoord *(_velocity[1]/_blength[0]);
        }
        else if(Flag(it.Down()) == ' '){
             v->Cell(it) = -4.0*xcoord*xcoord * (_velocity[1]/(_blength[0]*_blength[0])) + 4* xcoord *(_velocity[1]/_blength[0]);
        }
        else if(Flag(it.Right()) == ' '){
            std::cout << "V Vertical Boundary sollte so nicht auftauchen " << std::endl;
        }
        else if(Flag(it.Left()) == ' '){
            std::cout << "V Vertical Boundary sollte so nicht auftauchen " << std::endl;
        }
        
        else {
         //   std::cout << "Set_V_horizontal Problem" << std::endl;
        }
    }

    void Geometry::Set_V_noslip(Iterator it, Grid* v)const{
        if(Flag(it.Top()) == ' '){
            v->Cell(it) = 0;
            //v->Cell(it.Down()) = 0;
        }
        else if(Flag(it.Down()) == ' '){
            if (Flag(it.Left()) == ' '){
                v->Cell(it) = -v->Cell(it.Left());
                v->Cell(it.Down()) = 0;
            }
            else if (Flag(it.Right())== ' '){
                v->Cell(it) = -v->Cell(it.Right());
                v->Cell(it.Down()) = 0;
            }
            else{
                v->Cell(it) = 0;
                v->Cell(it.Down()) = 0;
            }
        }
        else if(Flag(it.Right()) == ' '){
            v->Cell(it) = -1.0 * v->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            v->Cell(it)= - v->Cell(it.Left());
        }
        else {
            v->Cell(it) = 0;
          //  std::cout << "Set_V_noslip cornerpoint at it: "<< it << std::endl;
        }
    }

    void Geometry::Set_V_outflow(Iterator it, Grid *v)const{
        if(Flag(it.Top()) == ' '){
            v->Cell(it) = v->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            v->Cell(it) = v->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            v->Cell(it.Left()) = v->Cell(it.Left().Left());
            v->Cell(it) = v->Cell(it.Left().Left());
        }
        else if(Flag(it.Down()) == ' '){
            v->Cell(it.Down())= v->Cell(it.Down().Down());
            v->Cell(it)= v->Cell(it.Down().Down());
        }
        else {
         //   std::cout << "Set_V_outflow Problem" << std::endl;
        }
    }
    void Geometry::Set_V_slipV(Iterator it, Grid* v)const{
        if(Flag(it.Top()) == ' '){
            v->Cell(it) = v->Cell(it.Top());
        }
        else if (Flag(it.Down()) == ' '){
            v->Cell(it.Down()) = v->Cell(it.Down().Down());
            v->Cell(it) = v->Cell(it.Down().Down());
        }
        else if(Flag(it.Right()) == ' '){
            v->Cell(it) = v->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            v->Cell(it) = v->Cell(it.Left());
        }
        else {
           // std::cout << "Set_V_slipV Problem" << std::endl;
        }
    }
    void Geometry::Set_V_slipH(Iterator it, Grid *v)const{
        if(Flag(it.Right()) == ' '){
            v->Cell(it) = - v->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            v->Cell(it) = - v->Cell(it.Left());
        }
        else if(Flag(it.Top()) == ' '){
            v->Cell(it) = 0;
        }
        else if(Flag(it.Down()) == ' '){
            v->Cell(it) = 0;
            v->Cell(it.Down()) = 0;
        }
        else {
          //  std::cout << "Set_V_slipH Problem" << std::endl;
        }
        
    }

    /////////////////////////////////////////////
    //P functions
    ////////////////////////////////////////////
    void Geometry::Set_P_vertical(Iterator it, Grid* p)const{
        if(Flag(it.Top()) == ' '){
            p->Cell(it) = p->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            p->Cell(it) = p->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            p->Cell(it) = p->Cell(it.Left());
        }
        else if(Flag(it.Down()) == ' '){
            p->Cell(it) = p->Cell(it.Down());
        }
        else {
         //   std::cout << "Set_V_vertical Problem" << std::endl;
        }
    }
    void Geometry::Set_P_horizontal(Iterator it, Grid* p)const{    
        if(Flag(it.Top()) == ' '){
            p->Cell(it) = p->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            p->Cell(it) = p->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            p->Cell(it) = p->Cell(it.Left());
        }
        else if(Flag(it.Down()) == ' '){
            p->Cell(it) = p->Cell(it.Down());
        }
        else {
         //   std::cout << "Set_V_horizontal Problem" << std::endl;
        }
    }
    void Geometry::Set_P_noslip(Iterator it, Grid* p)const{
        if( (Flag(it.Top()) == ' ') && (Flag(it.Right()) == ' ') ){
            p->Cell(it) = 0.5 * (p->Cell(it.Top()) + p->Cell(it.Right()) );
        }
        else if( (Flag(it.Top()) == ' ') && (Flag(it.Left()) == ' ') ){
            p->Cell(it) = 0.5 * (p->Cell(it.Top()) + p->Cell(it.Left()) );
        }
        else if( (Flag(it.Down()) == ' ') && (Flag(it.Left()) == ' ') ){
            p->Cell(it) = 0.5 * (p->Cell(it.Down()) + p->Cell(it.Left()) );
        }
        else if( (Flag(it.Down()) == ' ') && (Flag(it.Right()) == ' ') ){
            p->Cell(it) = 0.5 * (p->Cell(it.Down()) + p->Cell(it.Right()) );
        }
        //Nur eine FluidBoundary
        else if (Flag(it.Top()) == ' '){
            p->Cell(it) = p->Cell(it.Top());
        }
        else if(Flag(it.Down()) == ' '){
            p->Cell(it) = p->Cell(it.Down());
        }
        else if(Flag(it.Right()) == ' '){
            p->Cell(it) = p->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            p->Cell(it)= p->Cell(it.Left());
        }
        //Keine FluidBoundary
        else {
            p->Cell(it) = 0;
          //  std::cout << "Set_V_noslip cornerpoint at it: "<< it << std::endl;
        }
    }
    void Geometry::Set_P_outflow(Iterator it, Grid *p)const{
        if(Flag(it.Top()) == ' '){
            p->Cell(it) = 0;
        }
        else if(Flag(it.Right()) == ' '){
            p->Cell(it) = 0;
        }
        else if(Flag(it.Left()) == ' '){
            //p->Cell(it.Left()) = 0;
            p->Cell(it) = 0;
        }
        else if(Flag(it.Down()) == ' '){
            //p->Cell(it.Down())= 0;
            p->Cell(it) = 0;
        }
        else {
            std::cout << "Set_V_outflow Problem" << std::endl;
        }
    }
    void Geometry::Set_P_slipV(Iterator it, Grid* p)const{
        if(Flag(it.Top()) == ' '){
            p->Cell(it) = _pressure;
        }
        else if(Flag(it.Down()) == ' '){
            p->Cell(it) = _pressure;
        }
        if(Flag(it.Right()) == ' '){
            p->Cell(it) = p->Cell(it.Right());
            std::cout << "Boundary sollte es nicht geben in Set_P_slipV" << std::endl;
        }
        else if(Flag(it.Left()) == ' '){
            p->Cell(it) = p->Cell(it.Left());
            std::cout << "Boundary sollte es nicht geben in Set_P_slipV" << std::endl;
        }
        else {
            std::cout << "Set_V_slipV Problem" << std::endl;
        }
    }
    void Geometry::Set_P_slipH(Iterator it, Grid *p)const{
        if(Flag(it.Left()) == ' '){
             p->Cell(it) = _pressure;
        }
        else if(Flag(it.Right()) == ' '){
             p->Cell(it) = _pressure;
        }
        else if(Flag(it.Top()) == ' '){
            p->Cell(it) = p->Cell(it.Top());
            std::cout << "Boundary sollte es nicht geben in Set_P_slipH" << std::endl;
        }
        else if(Flag(it.Down()) == ' '){
            p->Cell(it) = p->Cell(it.Down());
            std::cout << "Boundary sollte es nicht geben in Set_P_slipH" << std::endl;
        }
        else {
            std::cout << "Set_V_slipH Problem" << std::endl;
        }
        
    }
    void Geometry::Set_P_inflow(Iterator it, Grid* p)const{
        if(Flag(it.Top()) == ' '){
            p->Cell(it) = p->Cell(it.Top());
        }
        else if(Flag(it.Right()) == ' '){
            p->Cell(it) = p->Cell(it.Right());
        }
        else if(Flag(it.Left()) == ' '){
            //p->Cell(it.Left()) = 0;
            p->Cell(it) = p->Cell(it.Left());
        }
        else if(Flag(it.Down()) == ' '){
            //p->Cell(it.Down())= 0;
            p->Cell(it) = p->Cell(it.Down());
        }
        else {
            std::cout << "Set_V_Inflow Problem" << std::endl;
        }
    }
    
char &Geometry::Flag(const Iterator &it){
  return _flag[it.Value()];
}


const char &Geometry::Flag(const Iterator &it) const{
  return _flag[it.Value()];
}

void Geometry::printGeometry(){
    std::cout<<"Print Geometry:" << std::endl;
    std::cout<<"_freeGeom: " <<_freeGeom << std::endl;
    std::cout<<"_numInteriorBounds: " <<_numInteriorBounds <<std::endl;
    std::cout<<"_bsize: " <<_bsize[0] << "|"<<_bsize[1] <<std::endl;
    std::cout<<"_blength: " <<_blength[0] << "|"<<_blength[1] <<std::endl;
    std::cout<<"_size: " <<_size[0] << "|"<<_size[1] <<std::endl;
    std::cout<<"_length: " <<_length[0] << "|"<<_length[1] <<std::endl;
    std::cout<<"_mesh: " <<_mesh[0] << "|"<<_mesh[1] <<std::endl;
    std::cout<<"_velocity: " <<_velocity[0] << "|"<<_velocity[1] <<std::endl;
    std::cout<<"_pressure: " <<_pressure <<std::endl;
}
void Geometry::BoundaryUpdateCoarse(Grid* error, const Grid* residual){
    BoundaryIterator it = BoundaryIterator(this);
            if(_comm->isLeft()){
                it.SetBoundary(4);
                it.First();
                while(it.Valid()){
                    //error->Cell(it) = error->Cell(it.Right()) + Mesh()[0]*residual->Cell(it);
                    error->Cell(it) = error->Cell(it.Right()) - Mesh()[0]*residual->Cell(it);
                    it.Next();
                }
            }
            if(_comm->isBottom()){
                it.SetBoundary(1);
                it.First();
                while(it.Valid()){
                    //error->Cell(it) = error->Cell(it.Top()) + Mesh()[1]*residual->Cell(it);
                    error->Cell(it) = error->Cell(it.Top()) - Mesh()[1]*residual->Cell(it);
                    it.Next();
                }
            }
            if(_comm->isRight()){
                it.SetBoundary(2);
                it.First();
                while(it.Valid()){
                    //error->Cell(it) = error->Cell(it.Left()) + Mesh()[0]*residual->Cell(it);
                    error->Cell(it) = error->Cell(it.Left()) + Mesh()[0]*residual->Cell(it);
                    it.Next();
                }
            }
            if(_comm->isTop()){
                it.SetBoundary(3);
                it.First();
                while(it.Valid()){
                    //error->Cell(it) = error->Cell(it.Down()) + Mesh()[1]*residual->Cell(it);
                    error->Cell(it) = error->Cell(it.Down()) + Mesh()[1]*residual->Cell(it);
                    it.Next();
                }
            }
}
