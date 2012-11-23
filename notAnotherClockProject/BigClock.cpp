#include "BigClock.h"

#include <U8glib.h>
#include <DS1307.h>
#include "tz.h"

#define BUFF_DATE 3

extern U8GLIB_ST7920_128X64 *lcd;

BigClock::BigClock():
Mode(PSTR("Big Clock")){
}

void BigClock::loop(){
  btime_t date;
  DS1307 time(TIMEZONE);

  // Date
  date=time.getBTime();

  // LCD
  lcd->firstPage();
  lcd->setFont(u8g_font_osb35);
  lcd->setFontPosTop();
  do {
    byte z;
    int x;
    int y;
    char buff[BUFF_DATE];
    z=0;  
    if(date.min<10)
      z=lcd->getStrWidthP(U8G_PSTR("0"));
    x=lcd->getWidth()/2-(
    lcd->getStrWidth(itoa(date.hour, buff, 10))+
      lcd->getStrWidthP(U8G_PSTR(":"))+
      z+
      lcd->getStrWidth(itoa(date.min, buff, 10))
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
    lcd->drawStr(x, y, itoa(date.min, buff, 10));
  }
  while( lcd->nextPage() );
}
