#include "Clock.h"

#include <Arduino.h>

void SerialPrintPGM (PGM_P s);

PGM_P * Clock::enter(){
  SerialPrintPGM(PSTR("Clock::enter()\n"));
  return NULL;
}

void Clock::loop(){
  SerialPrintPGM(PSTR("Clock::loop()\n"));
}

void Clock::exit(){
  SerialPrintPGM(PSTR("Clock::exit()\n"));
}




