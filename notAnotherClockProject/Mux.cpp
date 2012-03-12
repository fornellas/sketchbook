#include <Arduino.h>

#include "Mux.h"
#include "pins.h"

namespace Mux {
 void setAddr(byte addr);
}

void Mux::setAddr(byte addr){
  digitalWrite(PIN_CD74HC4067_S0, addr & 0x1);
  digitalWrite(PIN_CD74HC4067_S1, addr & 0x2);
  digitalWrite(PIN_CD74HC4067_S2, addr & 0x4);
  digitalWrite(PIN_CD74HC4067_S3, addr & 0x8);
}

int Mux::aRead(byte addr){
  pinMode(PIN_CD74HC4067_SIG_A, INPUT);
  setAddr(addr);
  return analogRead(PIN_CD74HC4067_SIG_A);
};
int Mux::dRead(byte addr){
  pinMode(PIN_CD74HC4067_SIG_D, INPUT);
  digitalWrite(PIN_CD74HC4067_SIG_D, HIGH);
  setAddr(addr);
  return digitalRead(PIN_CD74HC4067_SIG_D);
};
  
void Mux::dWrite(byte addr){
  pinMode(PIN_CD74HC4067_SIG_D, OUTPUT);
  digitalWrite(PIN_CD74HC4067_SIG_D, HIGH);
  setAddr(addr);
};

