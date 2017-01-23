#include "parameter.hpp"
using namespace std;
Parameter::Parameter(const char * paramInput)
{
  _re = 0;
  _omega = 0;
   _alpha = 0;
   _dt = 0;
   _tend = 0;
   _eps = 0;
   _tau = 0;
   _itermax = 0;
   Load(paramInput);
   //std::cout << "Parameter Constructor vollendet"<<std::endl;
}

  /// Loads the parameter values from a file
  void Parameter::Load(const char *file)
  {
      std::ifstream ParamFile (file);
      std::string line;
      index_t delPos;
      string variableName;
      string variableValue;
      if (ParamFile)
      {
          //cout << "ParameterFile eingelesen"<<endl;
          while (getline ( ParamFile, line))
          {
              // only for space=space in the parameter file
                delPos = line.find(" ");
                variableName = line.substr(0, delPos);
                variableValue = line.substr(delPos+3, line.length()-1);
                
                if(variableName.compare("re")== 0) {
                    _re = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " re = " << _re << endl;
                }
                else if(variableName.compare("omg")== 0) {
                    _omega = atof(variableValue.c_str());
                    //cout<< "variableName =  " << variableName << " omega = " << _omega << endl;
                }
                else if(variableName.compare("alpha")== 0) {
                    _alpha = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " alpha " << _alpha << endl;
                }
                else if(variableName.compare("dt")== 0) {
                    _dt = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " dt " << _dt << endl;
                }
                else if(variableName.compare("tend")== 0) {
                    _tend = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " tend = " << _tend << endl;
                }
                else if(variableName.compare("iter")== 0) {
                    _itermax = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " itermax " << _itermax << endl;
                }
                else if(variableName.compare("eps")== 0) {
                    _eps = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " epsilon= " << _eps << endl;
                }
                else if(variableName.compare("tau")== 0) {
                    _tau = atof(variableValue.c_str());
                    //cout<< "variableName = " << variableName << " tau= " << _tau << endl;
                }
                else if(variableName.compare("numOfRef") == 0){
                    _numOfRef = atof(variableValue.c_str());
                }
                else 
                {
                    cout << variableName << " aus Parameter File konnte nicht korrekt eingelesen werden"<<endl;
                }

              
        }
          ParamFile.close();
//           cout<< " re = " << _re << endl;
//           cout<< " omega = " << _omega << endl;
//           cout<< " alpha = " << _alpha << endl;
//           cout<< " dt = " << _dt << endl;
//           cout<< " tend = " << _tend << endl;
//           cout<< " itermax = " << _itermax << endl;
//           cout<< " eps = " << _eps << endl;
//           cout<< " tau = " << _tau << endl;
      }
      else 
      {
          std::cout << "Parameter File konnte nicht gelesen werden"<< std::endl;
      }
  }

  /// Getter functions for all parameters
  const real_t &Parameter::Re() const
  {
        return _re;
  }
  const real_t &Parameter::Omega() const
  {
      return _omega;
  }
      
  const real_t &Parameter::Alpha() const
  {
      return _alpha;
  }
  const real_t &Parameter::Dt() const
  {
      return _dt;
  }
  const real_t &Parameter::Tend() const
  {
      return _tend;
  }
  const index_t &Parameter::IterMax() const
  {
      return _itermax;
  }
  const real_t &Parameter::Eps() const
  {
      return _eps;
  }
  const real_t &Parameter::Tau() const
  {
      return _tau;
  }
  const index_t &Parameter::NumOfRef() const{
      return _numOfRef;
  }
