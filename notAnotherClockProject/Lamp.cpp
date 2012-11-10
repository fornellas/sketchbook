#include "Lamp.h"

#include <SFRGBLEDMatrix.h>
#include <avr/pgmspace.h>

extern SFRGBLEDMatrix *ledMatrix;

Lamp::Lamp():
Mode(PSTR("Lamp")){
  ledMatrix->fill(WHITE);
  ledMatrix->show();
}

void Lamp::loop(){

}




