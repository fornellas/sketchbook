#include "Lamp.h"

#include <Arduino.h>

void SerialPrintPGM (PGM_P s);

PGM_P * Lamp::enter(){
  SerialPrintPGM(PSTR("Lamp::enter()\n"));
  return NULL;
}

void Lamp::loop(){
  SerialPrintPGM(PSTR("Lamp::loop()\n"));
}

void Lamp::exit(){
  SerialPrintPGM(PSTR("Lamp::exit()\n"));
}




