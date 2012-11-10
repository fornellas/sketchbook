#include "Mode.h"

#include <SFRGBLEDMatrix.h>
#include <U8glib.h>

extern SFRGBLEDMatrix *ledMatrix;
extern U8GLIB_ST7920_128X64 *lcd;

Mode::Mode(PROGMEM char *name) {
  this->name=name;
  // Clear LCD
  lcd->firstPage();
  do {
  } 
  while( lcd->nextPage() );
  // TV Out LED Matrix
  ledMatrix->gamma(false); 
  for(byte p=0;p<(ledMatrix->height<ledMatrix->width?ledMatrix->height:ledMatrix->width)/2;p++){
    ledMatrix->box(BLACK,p, p, ledMatrix->width-1-p, ledMatrix->height-1-p);
    ledMatrix->show();
  }  
  delay(300);
  // Draw name on LCD
  lcd->setColorIndex(1);
  lcd->firstPage();
  do {
    byte x;
    byte y;
    x=lcd->getWidth()/2-lcd->getStrWidthP((u8g_pgm_uint8_t *)name)/2;
    y=lcd->getHeight()/2+lcd->getFontAscent()/2;
    lcd->drawStrP(x, y, (u8g_pgm_uint8_t *)name);
  } 
  while( lcd->nextPage() );
  delay(400);
  //TODO entrance animation
}

Mode::~Mode() {
  // TODO ending animation
}






