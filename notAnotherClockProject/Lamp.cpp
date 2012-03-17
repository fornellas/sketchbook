#include "Lamp.h"
#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include "pins.h"


extern SFRGBLEDMatrix *display;
extern Button *button;

Lamp::Lamp(){
  Serial.println("Lamp()");
}

Lamp::~Lamp(){
  Serial.println("~Lamp()");
}

PGM_P *Lamp::getName(){

}

void Lamp::loop(){
  Serial.println("Lamp::loop()");
}

