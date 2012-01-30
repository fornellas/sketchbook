#include "RGBLED.h"
#include "pins.h"

#define DELAY 50

void RGBLED::exit(){
  analogWrite(PIN_RGBLED_R, 0);
  analogWrite(PIN_RGBLED_G, 0);
  analogWrite(PIN_RGBLED_B, 0);
}

void RGBLED::loop() {
  static int g;
  g++;
  Serial.println(g);
  for(;g<256;g++){
    analogWrite(PIN_RGBLED_G, g);
    return;
  }
  delay(DELAY);
  for(static int r=255;r>-1;r--){
    analogWrite(PIN_RGBLED_R, r);
    return;
  }
  delay(DELAY);
  for(static int b=0;b<256;b++){
    analogWrite(PIN_RGBLED_B, b);
    return;
  }
  delay(DELAY);
  for(static int g=255;g>-1;g--){
    analogWrite(PIN_RGBLED_G, g);
    return;
  }
  delay(DELAY);
  for(static int r=0;r<256;r++){
    analogWrite(PIN_RGBLED_R, r);
    return;
  }
  delay(DELAY);
  for(static int b=255;b>-1;b--){
    analogWrite(PIN_RGBLED_B, b);
    return;
  }
  delay(DELAY);
}

