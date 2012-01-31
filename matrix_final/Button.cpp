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

Button::Button() {
  for(byte b=0;b<BUTTON_COUNT;b++){
    buttonState[b]=false;
    lastButtonState[b]=false;
    lastDebounceTime[b]=0;
    pressedState[b]=false;
    releasedState[b]=false;
    pinMode(pgm_read_word_near(pin+b), INPUT);
    digitalWrite(pgm_read_word_near(pin+b), HIGH);
  }
}

void Button::update(){
  for(byte b=0;b<BUTTON_COUNT;b++){
    int reading=!digitalRead(pgm_read_word_near(pin+b));
    if (reading != lastButtonState[b])
      lastDebounceTime[b] = millis();
    if ((millis() - lastDebounceTime[b]) > DEBOUNCE_DELAY) {
      pressedState[b]=false;
      releasedState[b]=false;
      if(!buttonState[b]&&reading)
        pressedState[b]=true;
      if(buttonState[b]&&!reading)
        releasedState[b]=true;
      buttonState[b] = reading;
    }
    lastButtonState[b] = reading;
  }
}

boolean Button::state(byte button){
  return buttonState[button];
};

boolean Button::pressed(byte button){
  return pressedState[button];
}

boolean Button::released(byte button){
  return releasedState[button]; 
}










