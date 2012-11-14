#include "BigClock.h"

#include <U8glib.h>
#include <DS1307.h>


extern U8GLIB_ST7920_128X64 *lcd;

BigClock::BigClock():
Mode(PSTR("Big Clock")){
}

void BigClock::loop(){
  struct DS1307::Date date;

  // Date
  date=DS1307::getDate();

  // LCD
  lcd->firstPage();
  lcd->setFont(u8g_font_osb35);
  lcd->setFontPosTop();
  do {
    byte z;
    int x;
    int y;
    char buff[10];
    z=0;  
    if(date.minute<10)
      z=lcd->getStrWidthP(U8G_PSTR("0"));
    x=lcd->getWidth()/2-(
    lcd->getStrWidth(itoa(date.hour, buff, 10))+
      lcd->getStrWidthP(U8G_PSTR(":"))+
      z+
      lcd->getStrWidth(itoa(date.minute, buff, 10))
      )/2;
    y=lcd->getHeight()/2-lcd->getFontAscent()/2;
    lcd->drawStr(x, y, itoa(date.hour, buff, 10));
    x+=lcd->getStrWidth(itoa(date.hour, buff, 10));
    lcd->drawStrP(x, y, U8G_PSTR(":"));
    x+=lcd->getStrWidthP(U8G_PSTR(":"));
    if(z){
      lcd->drawStrP(x, y, U8G_PSTR("0"));
      x+=lcd->getStrWidthP(U8G_PSTR("0"));
    }
    lcd->drawStr(x, y, itoa(date.minute, buff, 10));
  }
  while( lcd->nextPage() );
}
