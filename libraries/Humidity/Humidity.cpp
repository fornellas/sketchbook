#include "Humidity.h"

Humidity::Humidity(){
  loadFromSensor();
}

double Humidity::getRH(){
  return RH;
}
