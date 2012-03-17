#include "Fire.h"
#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include "pins.h"

extern SFRGBLEDMatrix *display;
extern Button *button;

Fire::Fire(){
  Serial.println("Fire()");
}

Fire::~Fire(){
  Serial.println("~Fire()");
}

PGM_P *Fire::getName(){

}

void Fire::loop(){
  Serial.println("Fire::loop()");
}

