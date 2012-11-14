#include "Mode.h"
#include "Light.h"

#include <SFRGBLEDMatrix.h>
#include <U8glib.h>

extern SFRGBLEDMatrix *ledMatrix;
extern U8GLIB_ST7920_128X64 *lcd;
extern Light *light;

Mode::Mode(PROGMEM char *name) {
  byte x;
  byte y;
  this->name=name;
  // Draw name on LCD
  lcd->setFont(u8g_font_fub14);
  lcd->setFontPosBaseline();
  x=lcd->getWidth()/2-lcd->getStrWidthP((u8g_pgm_uint8_t *)name)/2;
  y=lcd->getHeight()/2+lcd->getFontAscent()/2;
  lcd->firstPage();
  do {
    lcd->drawStrP(x, y, (u8g_pgm_uint8_t *)name);
  } 
  while( lcd->nextPage() );
  // CRT Out LED Matrix
  ledMatrix->CRT(BLACK, BLACK);
}

Mode::~Mode() {
  // clear LCD
  lcd->firstPage();
  do {
  }
  while( lcd->nextPage() );
  ledMatrix->gamma(false); 
  return;
}












