#include "Button.h"
#include "pins.h"
#include <Arduino.h>
#include <avr/pgmspace.h>

#define DEBOUNCE_DELAY 30

prog_int16_t pin[BUTTON_COUNT] PROGMEM = {
  PIN_BUTTON_MODE,
  PIN_BUTTON_A,
  PIN_BUTTON_B,
};

prog_int16_t addr_pin[BUTTON_COUNT] PROGMEM = {
  ADDR_BUTTON_MODE,
  ADDR_BUTTON_A,
  ADDR_BUTTON_B,
};

Button::Button() {
  for(byte b=0;b<BUTTON_COUNT;b++){
    buttonState[b]=false;
    lastChangeTime[b]=0;
    pressedState[b]=false;
    releasedState[b]=false;
  }
}

void Button::update(){
  for(byte b=0;b<BUTTON_COUNT;b++){
    unsigned long currTime=millis();
    int reading;

    pinMode(pgm_read_word_near(pin+b), INPUT);
    digitalWrite(pgm_read_word_near(pin+b), HIGH);
    digitalWrite(PIN_CD74HC4067_S0, pgm_read_word_near(addr_pin+b)&0x1);
    digitalWrite(PIN_CD74HC4067_S1, pgm_read_word_near(addr_pin+b)&0x2);
    digitalWrite(PIN_CD74HC4067_S2, pgm_read_word_near(addr_pin+b)&0x4);
    digitalWrite(PIN_CD74HC4067_S3, pgm_read_word_near(addr_pin+b)&0x8);
    delay(1);
    reading=!digitalRead(pgm_read_word_near(pin+b));
    if (reading != buttonState[b] && currTime - lastChangeTime[b] > DEBOUNCE_DELAY){
      lastChangeTime[b] = currTime;
      pressedState[b]=false;
      releasedState[b]=false;
      if(!buttonState[b]&&reading)
        pressedState[b]=true;
      if(buttonState[b]&&!reading)
        releasedState[b]=true;
      buttonState[b] = reading;
    }
  }
}

boolean Button::state(byte button){
  return buttonState[button];
};

boolean Button::pressed(byte button){
  boolean r;
  r=pressedState[button];
  pressedState[button]=false;
  return r;
}

boolean Button::released(byte button){
  boolean r;
  r=releasedState[button];
  releasedState[button]=false;
  return r; 
}













