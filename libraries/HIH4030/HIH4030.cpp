#include "HIH4030.h"
#include <Arduino.h>

HIH4030::HIH4030(uint8_t sensorPin, Temperature *temp){
  pin=sensorPin;
  temperature=temp;
  pinMode(pin, INPUT);
}

void HIH4030::loadFromSensor(){
  analogReference(DEFAULT);
  RH=((float(analogRead(pin))/1023.0)-0.16)/(0.00653852-0.000013392*temperature->getC());
}
