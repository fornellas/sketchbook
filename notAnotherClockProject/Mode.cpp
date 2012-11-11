#include "Mode.h"
#include "Light.h"

#include <SFRGBLEDMatrix.h>
#include <U8glib.h>

extern SFRGBLEDMatrix *ledMatrix;
extern U8GLIB_ST7920_128X64 *lcd;
extern Light *light;

Mode::Mode(PROGMEM char *name) {
  this->name=name;
  // Draw name on LCD
  lcd->firstPage();
  do {
    byte x;
    byte y;
    x=lcd->getWidth()/2-lcd->getStrWidthP((u8g_pgm_uint8_t *)name)/2;
    y=lcd->getHeight()/2+lcd->getFontAscent()/2;
    lcd->drawStrP(x, y, (u8g_pgm_uint8_t *)name);
  } 
  while( lcd->nextPage() );

  // CRT Out LED Matrix
  ledMatrix->gamma(false); 
  for(byte p=0;p<ledMatrix->height/2-1;p++){
    if(p)
      ledMatrix->box(BLACK, p-1, p-1, ledMatrix->width-1-p+1, ledMatrix->height-1-p+1);
    ledMatrix->box(RGB(light->read(MAX_C), light->read(MAX_C), light->read(MAX_C)), p, p, ledMatrix->width-1-p, ledMatrix->height-1-p);
    ledMatrix->show();
  }  
  for(byte p=ledMatrix->height/2-1;p<ledMatrix->width/2;p++){
    ledMatrix->box(BLACK, p-1, ledMatrix->height/2-2-1, ledMatrix->width-1-p+1, ledMatrix->height/2+1+1);
    ledMatrix->box(RGB(light->read(MAX_C), light->read(MAX_C), light->read(MAX_C)), p, ledMatrix->height/2-2, ledMatrix->width-1-p, ledMatrix->height/2+1);
    ledMatrix->show();
  }
  for(byte p=ledMatrix->height/2-2;p<ledMatrix->height/2;p++){
    ledMatrix->box(BLACK, ledMatrix->width/2-2, p, ledMatrix->width/2+1, ledMatrix->height-p-1);
    ledMatrix->show();
  }
}

Mode::~Mode() {
  // TODO ending animation
}










