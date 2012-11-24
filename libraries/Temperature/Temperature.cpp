#include "Temperature.h"

Temperature::Temperature(){
  loadFromSensor();
}

// return values

double Temperature::getK(){
  return K;
}

double Temperature::getC(){
  return K-273.15;
}

double Temperature::getF(){
  return K*9/5-459.57;
}
