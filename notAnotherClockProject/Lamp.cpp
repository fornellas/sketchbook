#include "Lamp.h"

#include "facilities.h"

Lamp::Lamp():
Mode(PSTR("Lamp")){
  ledMatrix->fill(WHITE);
  ledMatrix->show();
}

void Lamp::loop(){

}




