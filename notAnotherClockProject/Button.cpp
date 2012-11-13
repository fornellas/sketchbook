#include <Arduino.h>
#include <avr/pgmspace.h>

#include "Button.h"
#include "pins.h"

#define DEBOUNCE_DELAY 50

prog_int16_t pin[BUTTON_COUNT] PROGMEM = {
  PIN_BUTTON_MODE,
  PIN_BUTTON_A,
  PIN_BUTTON_B,
  PIN_BUTTON_C,
};

Button::Button() {
  // states
  for(byte b=0;b<BUTTON_COUNT;b++){
    buttonState[b]=true;
    lastChangeTime[b]=0;
    pressedState[b]=false;
    releasedState[b]=false;
  }

  for(byte b=0;b<BUTTON_COUNT;b++){
    uint8_t addr;
    addr=pgm_read_word_near(pin+b);
    // pull-ups
    pinMode(addr, INPUT);
    digitalWrite(addr, HIGH);
  }
}

void Button::update(){
  for(byte b=0;b<BUTTON_COUNT;b++){
    unsigned long currTime=millis();
    int reading;

    reading=digitalRead(pgm_read_word_near(pin+b));
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
  return !buttonState[button];
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





