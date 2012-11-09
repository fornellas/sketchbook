#include "Spectrum.h"

#include <Arduino.h>
#include "Mux.h"
#include "pins.h"

void SerialPrintPGM (PGM_P s);

int min=1023;
int max=0;
long int avg;

PGM_P * Spectrum::enter(){
  SerialPrintPGM(PSTR("Spectrum::enter()\n"));
//  Mux::setPin(ADDR_MIC, INPUT, 0);
  avg=0;
  min=1023;
  max=0;
  for(int c=0;c<1000;c++)
//    avg+=Mux::aRead();
  avg+=analogRead(PIN_LINEIN);
  avg=avg/1000;
  return NULL;
}

void Spectrum::loop(){
  int r;
  Mux::setPin(ADDR_MIC, INPUT, 0);
  //r=Mux::aRead();
  r=analogRead(PIN_LINEIN);
  if(r<min)min=r;
  if(r>max)max=r;

  Serial.print(min);
  Serial.print(' ');
//  Serial.print(avg);
//  Serial.print(' ');
  Serial.println(max);
//  Serial.print(' ');
//  Serial.print(r);
  delay(100);
}

void Spectrum::exit(){
  SerialPrintPGM(PSTR("Spectrum::exit()\n"));
}









