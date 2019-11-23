#ifndef AUXILIARY_FUNCTION_H
#define AUXILIARY_FUNCTION_H

#include <string>
#include <iostream>

using namespace std;

// class AuxiliaryFunction
// {
// public:
  string ConvertSize(int n, int tam){
      string size = to_string(n);
      if(size.length() < tam){
          size = string(tam - size.length(), '0') + size;
      }
      return size;
   }
// };


#endif
