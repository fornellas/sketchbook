#include "Plasma.h"

#include <Arduino.h>

void SerialPrintPGM (PGM_P s);

PGM_P * Plasma::enter(){
  SerialPrintPGM(PSTR("Plasma::enter()\n"));
  return NULL;
}

void Plasma::loop(){
  SerialPrintPGM(PSTR("Plasma::loop()\n"));
}

void Plasma::exit(){
  SerialPrintPGM(PSTR("Plasma::exit()\n"));
}




