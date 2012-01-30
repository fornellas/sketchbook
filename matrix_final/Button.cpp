#include "Button.h"
#include "pins.h"
#include <Arduino.h>
#include <avr/pgmspace.h>

#define DEBOUNCE_DELAY 30

prog_int16_t pin[BUTTON_COUNT] PROGMEM = {
  DIN_BUTTON_MODE,
  DIN_BUTTON_A,
  DIN_BUTTON_B,
};

Button::Button() {
  for(byte b=0;b<BUTTON_COUNT;b++){
    buttonState[b]=false;
    lastButtonState[b]=false;
    lastDebounceTime[b]=0;
    pinMode(pgm_read_word_near(pin+b), INPUT);
    digitalWrite(pgm_read_word_near(pin+b), HIGH);
  }
}

void Button::update(){
  for(byte b=0;b<BUTTON_COUNT;b++){
    int reading=!digitalRead(pgm_read_word_near(pin+b));
//    Serial.println(reading);
    if (reading != lastButtonState[b])
      lastDebounceTime[b] = millis();
    if ((millis() - lastDebounceTime[b]) > DEBOUNCE_DELAY)
      buttonState[b] = reading;
    lastButtonState[b] = reading;
  }
}

boolean Button::state(byte button){
  return buttonState[button];
};






