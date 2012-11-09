#include "Fire.h"

#include <Arduino.h>

void SerialPrintPGM (PGM_P s);

PGM_P * Fire::enter(){
  SerialPrintPGM(PSTR("Fire::enter()\n"));
  return NULL;
}

void Fire::loop(){
  SerialPrintPGM(PSTR("Fire::loop()\n"));
}

void Fire::exit(){
  SerialPrintPGM(PSTR("Fire::exit()\n"));
}




