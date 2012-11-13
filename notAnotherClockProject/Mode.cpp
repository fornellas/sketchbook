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
  // Clear screen
  ledMatrix->clear();
  ledMatrix->show();
}

Mode::~Mode() {
  // CRT Out LED Matrix
  ledMatrix->gamma(false); 
  ledMatrix->CRT(BLACK, RGB(light->read(MAX_C), light->read(MAX_C), light->read(MAX_C)));
  return;
}










