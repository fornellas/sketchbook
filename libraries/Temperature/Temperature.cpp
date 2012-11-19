#include "Temperature.h"

double Temperature::readC(){
  return readK()-273.15;
}

double Temperature::readF(){
  return readK()*9/5-459.57;
}

double Temperature::convC2F(double C){
  return C*9/5+32;
}

double Temperature::convC2K(double C){
  return C+273.15;
}
