#include <Arduino.h>
#include <avr/pgmspace.h>

#include "Button.h"

#define DEBOUNCE_DELAY 50

prog_int16_t addr_pin[BUTTON_COUNT] PROGMEM = {
  ADDR_BUTTON_MODE,
  ADDR_BUTTON_A,
  ADDR_BUTTON_B,
  ADDR_BUTTON_C,
};

Button::Button() {
  for(byte b=0;b<BUTTON_COUNT;b++){
    buttonState[b]=true;
    lastChangeTime[b]=0;
    pressedState[b]=false;
    releasedState[b]=false;
  }
}

void Button::update(){
  for(byte b=0;b<BUTTON_COUNT;b++){
    unsigned long currTime=millis();
    int reading;

    reading=Mux::dRead(pgm_read_word_near(addr_pin+b));
    if (reading != buttonState[b] && currTime - lastChangeTime[b] > DEBOUNCE_DELAY){
      lastChangeTime[b] = currTime;
      pressedState[b]=false;
      releasedState[b]=false;
      if(buttonState[b]&&!reading)
        pressedState[b]=true;
      if(!buttonState[b]&&reading)
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

