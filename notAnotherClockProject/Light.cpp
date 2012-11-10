#include "Light.h"

#include <Arduino.h>
#include <math.h>

Light::Light(uint8_t pin){
  this->pin=pin;
  pinMode(pin, INPUT);

  total=0;
  for(byte c=0;c<NUMREADINGS;c++){
    readings[c]=analogRead(pin);
    total+=readings[c];
  }
  index=0;
}

void Light::update(){
  total-=readings[index];
  readings[index]=analogRead(pin);
  total+=readings[index];
  if(++index==NUMREADINGS)
    index=0;
}

int Light::read(int m){
  return round(((double)total/(double)NUMREADINGS)*(double)m/(double)1023);;
}




