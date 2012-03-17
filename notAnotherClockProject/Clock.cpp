#include "Clock.h"
#include <DS1307.h>
#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include <EEPROM.h>
#include "EEPROM.h"
#include "pins.h"

extern SFRGBLEDMatrix *display;
extern Button *button;

Clock::Clock(){
  Serial.println("Clock()");
}

Clock::~Clock(){
    Serial.println("~Clock()");
}

PGM_P *Clock::getName(){
  
}

void Clock::loop(){
  Serial.println("Clock::loop()");
}
