#include <Arduino.h>

#include "Mux.h"
#include "pins.h"

void Mux::begin(){
  pinMode(PIN_CD74HC4067_S0, OUTPUT);
  pinMode(PIN_CD74HC4067_S1, OUTPUT);
  pinMode(PIN_CD74HC4067_S2, OUTPUT);
  pinMode(PIN_CD74HC4067_S3, OUTPUT); 
};

void Mux::setPin(byte addr, uint8_t mode, boolean analog){
  if(analog)
    pinMode(PIN_CD74HC4067_SIG_A, mode);
  else
    if(mode==INPUT){
      pinMode(PIN_CD74HC4067_SIG_D, INPUT);
      digitalWrite(PIN_CD74HC4067_SIG_D, HIGH);
    }
    else
      pinMode(PIN_CD74HC4067_SIG_D, OUTPUT);
  digitalWrite(PIN_CD74HC4067_S0, addr & 0x1);
  digitalWrite(PIN_CD74HC4067_S1, addr & 0x2);
  digitalWrite(PIN_CD74HC4067_S2, addr & 0x4);
  digitalWrite(PIN_CD74HC4067_S3, addr & 0x8);
};

int Mux::aRead(){
  return analogRead(PIN_CD74HC4067_SIG_A);
};

int Mux::aRead(byte addr){
  setPin(addr, INPUT, 1);
  return aRead();
};

int Mux::dRead(){
  return digitalRead(PIN_CD74HC4067_SIG_D);
}

int Mux::dRead(byte addr){
  setPin(addr, INPUT, 0);
  return dRead();
};

void Mux::dWrite(uint8_t value){
  digitalWrite(PIN_CD74HC4067_SIG_D, value);
};

void Mux::dWrite(uint8_t value, byte addr){
  setPin(addr, OUTPUT, 0);
  dWrite(value);
};





