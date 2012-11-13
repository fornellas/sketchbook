#include "Equalizer.h"

#include "pins.h"
#include <SFRGBLEDMatrix.h>
#include <Adafruit_MCP23017.h>
#include "Button.h"

extern Adafruit_MCP23017 ioexp;
extern SFRGBLEDMatrix *ledMatrix;
extern Button *button;

#define BANDS 7

// 600mV output offset
#define DC_OFFSET (0.6/5.0*1023.0)

Equalizer::Equalizer():
Mode(PSTR("Equalizer")){
  filter=1;

  pin=PIN_EQ_LINEIN;

  pinMode(PIN_EQ_STROBE, OUTPUT);
  digitalWrite(PIN_EQ_STROBE, LOW);

  ioexp.pinMode(ADDR_EQ_RESET, OUTPUT);
  ioexp.digitalWrite(ADDR_EQ_RESET, LOW);  

  // ADDR_EQ_RESET
  ioexp.digitalWrite(ADDR_EQ_RESET, HIGH);  
  // minimum pulse width 100ns
  delayMicroseconds(1);
  ioexp.digitalWrite(ADDR_EQ_RESET, LOW);  
  digitalWrite(PIN_EQ_STROBE, HIGH);
  // minimum delay after: 72us
  delayMicroseconds(72);
  digitalWrite(PIN_EQ_STROBE, LOW);
  
  // Better colors with gamma enabled
  ledMatrix->gamma(true);
}

void Equalizer::loop(){
  int value[BANDS];
  int c, v;

  if(button->pressed(BUTTON_A)) {
    if(pin==PIN_EQ_MIC)
      pin=PIN_EQ_LINEIN;
    else
      pin=PIN_EQ_MIC; 
  }

  // Read value
  for(c=0;c<BANDS;c++){
    // minimum delay before read: 36us
    delayMicroseconds(36);
    value[c]=analogRead(pin);
    // minimum PIN_EQ_STROBE to PIN_EQ_STROBE: 72us
    delayMicroseconds(18);
    digitalWrite(PIN_EQ_STROBE, HIGH);
    // minimum PIN_EQ_STROBE pulse width: 18us
    delayMicroseconds(18);
    digitalWrite(PIN_EQ_STROBE, LOW);
  }
  ledMatrix->fill(BLACK);
  for(c=0;c<BANDS;c++){
    if(value[c]<DC_OFFSET)
      value[c]=0;
    else
      value[c]-=DC_OFFSET;
    int clear=1;
    for(v=0;v<ledMatrix->height;v++){
      if((ledMatrix->height)*double(value[c])/(1023.0-DC_OFFSET)>v){
        clear=0;
        if(!filter){
          Color color;
          color=ledMatrix->spectrum(v+1, ledMatrix->height);
          ledMatrix->paintPixel(color, c*3+2, (ledMatrix->height)-v-1);        
          ledMatrix->paintPixel(color, c*3+3, (ledMatrix->height)-v-1);
        }
      }
      else
        break;
    }
    if(clear)
      filter=1;
    else
      filter=0;
  }

  ledMatrix->show(); 
}

