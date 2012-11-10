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

#define SPECTRUM_LEN 42
Color spectrum[]={
  // RED
  RGB(7, 1, 0), RGB(7, 2, 0), RGB(7, 3, 0), RGB(7, 4, 0), RGB(7, 5, 0), RGB(7, 6, 0), RGB(7, 7, 0), 
  // YELLOW
  RGB(6, 7, 0), RGB(5, 7, 0), RGB(4, 7, 0), RGB(3, 7, 0), RGB(2, 7, 0), RGB(1, 7, 0), RGB(0, 7, 0), 
  // GREEN
  RGB(0, 7, 1), RGB(0, 7, 1), RGB(0, 7, 2), RGB(0, 7, 2), RGB(0, 7, 2), RGB(0, 7, 3), RGB(0, 7, 3), 
  // CYAN
  RGB(0, 6, 3), RGB(0, 5, 3), RGB(0, 4, 3), RGB(0, 3, 3), RGB(0, 2, 3), RGB(0, 1, 3), RGB(0, 0, 3), 
  // BLUE
  RGB(1, 0, 3), RGB(2, 0, 3), RGB(3, 0, 3), RGB(4, 0, 3), RGB(5, 0, 3), RGB(6, 0, 3), RGB(7, 0, 3), 
  // MAGENTA
  RGB(7, 0, 2), RGB(7, 0, 2), RGB(7, 0, 1), RGB(7, 0, 1), RGB(7, 0, 1), RGB(7, 0, 0), RGB(7, 0, 0), 
};

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
          ledMatrix->paintPixel(spectrum[int(float(v)/(ledMatrix->height-1)*float(SPECTRUM_LEN-1))], c*3+2, (ledMatrix->height)-v-1);        
          ledMatrix->paintPixel(spectrum[int(float(v)/(ledMatrix->height-1)*float(SPECTRUM_LEN-1))], c*3+3, (ledMatrix->height)-v-1);
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

