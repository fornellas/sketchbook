#include "Clock.h"
#include "pins.h"

#include <SFRGBLEDMatrix.h>
#include <U8glib.h>
#include <DS1307.h>
#include <HIH4030.h>
#include <DS18S20.h>
#include <BMP085.h>
#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "Light.h"
#include "Button.h"
#include "EEPROM_addr.h"

extern SFRGBLEDMatrix *ledMatrix;
extern U8GLIB_ST7920_128X64 *lcd;
extern Light *light;
extern Button *button;

// LCD

void
Clock::lcdInfo(){
  struct DS1307::Date date;
  date=DS1307::getDate();
  float temperatureOutside;
  byte addr[]={
    40, 200, 10, 228, 3, 0, 0, 62                                                                                                                                                            };
  temperatureOutside=DS18S20::read(PIN_TEMP_EXT, addr);
  float temperatureInside;
  temperatureInside=BMP085::readTemperature();
  byte humidity;
  humidity=HIH4030::read(PIN_HUMIDITY, temperatureInside);
  long pressure=BMP085::readPressure()/100;
  char buff[10]; // revisar tamanho
  // LCD
  lcd->firstPage();
  do {
    byte x;
    byte y;
    // HH:MM
    byte z;
    z=0;  
    if(date.minute<10)
      z=lcd->getStrWidthP(U8G_PSTR("0"));
    x=lcd->getWidth()/2-(
    lcd->getStrWidth(itoa(date.hour, buff, 10))+
      lcd->getStrWidthP(U8G_PSTR(":"))+
      z+
      lcd->getStrWidth(itoa(date.minute, buff, 10))
      )/2;
    y=lcd->getFontAscent();
    lcd->drawStr(x, y, itoa(date.hour, buff, 10));
    x+=lcd->getStrWidth(itoa(date.hour, buff, 10));
    lcd->drawStrP(x, y, U8G_PSTR(":"));
    x+=lcd->getStrWidthP(U8G_PSTR(":"));
    if(z){
      lcd->drawStrP(x, y, U8G_PSTR("0"));
      x+=lcd->getStrWidthP(U8G_PSTR("0"));
    }
    lcd->drawStr(x, y, itoa(date.minute, buff, 10));
    // Week day
    // Day, month year
    // Temperature outsite
    x=0;
    y=lcd->getHeight();
    lcd->drawStr(x, y, dtostrf(temperatureOutside, -2, 1, buff));
    x+=lcd->getStrWidth(dtostrf(temperatureOutside, -2, 1, buff));
    buff[0]=176;
    buff[1]='C';
    buff[2]='\0';
    lcd->drawStr(x, y, buff);
    // Temperature inside
    x=0;
    y=lcd->getHeight()-lcd->getFontAscent()-1;
    lcd->drawStr(x, y, dtostrf(temperatureInside, -2, 1, buff));
    x+=lcd->getStrWidth(dtostrf(temperatureInside, -2, 1, buff));
    buff[0]=176;
    buff[1]='C';
    buff[2]='\0';
    lcd->drawStr(x, y, buff);
    // Pressure
    x=lcd->getWidth()-lcd->getStrWidth(ltoa(pressure, buff, 10))-
      lcd->getStrWidthP(U8G_PSTR("hPa"));
    y=lcd->getHeight()-lcd->getFontAscent()-1;
    lcd->drawStr(x, y, ltoa(pressure, buff, 10));
    x+=lcd->getStrWidth(ltoa(pressure, buff, 10));
    lcd->drawStrP(x, y, U8G_PSTR("hPa"));
    // Humidity
    x=lcd->getWidth()-lcd->getStrWidth(itoa(humidity, buff, 10))-
      lcd->getStrWidthP(U8G_PSTR("%"));
    y=lcd->getHeight();
    lcd->drawStr(x, y, itoa(humidity, buff, 10));
    x+=lcd->getStrWidth(itoa(humidity, buff, 10));
    lcd->drawStrP(x, y, U8G_PSTR("%"));

  } 
  while( lcd->nextPage() );
}

// Digital

void Clock::showDigital(){
  int xOffset;
  int yOffset;
  xOffset=(ledMatrix->width>>1)-8;
  yOffset=(ledMatrix->height>>1)-11/2-1;
  ledMatrix->print(RGB(value,0,0), xOffset+0, yOffset+0, 5, 48+(date.hour/10));
  ledMatrix->print(RGB(value,0,0), xOffset+4, yOffset+0, 5, 48+(date.hour%10));
  ledMatrix->print(RGB(0,value,0), xOffset+8, yOffset+0, 5, 48+(date.minute/10));
  ledMatrix->print(RGB(0,value,0), xOffset+12, yOffset+0, 5, 48+(date.minute%10));
  ledMatrix->print(RGB(0,0,value), xOffset+4, yOffset+7, 5, 48+(date.second/10));
  ledMatrix->print(RGB(0,0,value), xOffset+8, yOffset+7, 5, 48+(date.second%10));
}

// Binary

void Clock::drawBCDdigit(byte value, int x, int y, byte digit){
  if(digit&(1<<3))
    ledMatrix->box(RGB(value, value, value), x, y, x+1, y+1);
  if(digit&(1<<2))
    ledMatrix->box(RGB(0, 0, value), x, y+3, x+1, y+1+3);
  if(digit&(1<<1))
    ledMatrix->box(RGB(0, value, 0), x, y+6, x+1, y+1+6);
  if(digit&1)
    ledMatrix->box(RGB(value, 0, 0), x, y+9, x+1, y+1+9);
}

void Clock::showBinary(){
  drawBCDdigit(value, 2, 3, date.hour/10);
  drawBCDdigit(value, 5, 3, date.hour%10);
  drawBCDdigit(value, 10, 3, date.minute/10);
  drawBCDdigit(value, 13, 3, date.minute%10);
  drawBCDdigit(value, 18, 3, date.second/10); 
  drawBCDdigit(value, 21, 3, date.second%10); 
  ledMatrix->paintPixel(RGB(value, value, value), 8, 6);
  ledMatrix->paintPixel(RGB(value, value, value), 8, 10);
  ledMatrix->paintPixel(RGB(value, value, value), 16, 6);
  ledMatrix->paintPixel(RGB(value, value, value), 16, 10);
}

// DX Time

void Clock::showDX() {
  Color centerColor=RGB(value,value,value);
  Color hourColor=RGB(value,0,0);
  Color minuteColor5=RGB(value,value,0);
  Color minuteColor1=RGB(0,value,0);
  Color secondColor=RGB(0,0,value);
  int xOff;
  xOff=4;
  // Middle point
  ledMatrix->paintPixel(centerColor, xOff+7, 7);
  ledMatrix->paintPixel(centerColor, xOff+8, 7);
  ledMatrix->paintPixel(centerColor, xOff+7, 8);
  ledMatrix->paintPixel(centerColor, xOff+8, 8);
  // Hour
  int hour;
  hour=date.hour;
  if(hour>=13)
    hour-=12;
  byte hx1[12] = {
    11,14,15,14,11,7,3,1,0,1,3,7                                                                                                            };
  byte hy1[12] = {
    1,3,7,11,14,15,14,11,7,3,1,0                                                                                                            };
  byte hx2[12] = {
    12,14,15,14,12,8,4,1,0,1,4,8                                                                                                            };
  byte hy2[12] = {
    1,4,8,12,14,15,14,12,8,4,1,0                                                                                                            };
  for(byte h=1;h<13;h++){
    if(hour>=h||hour==0) {
      ledMatrix->paintPixel(hourColor, xOff+hx1[h-1], hy1[h-1]);
      ledMatrix->paintPixel(hourColor, xOff+hx2[h-1], hy2[h-1]);
    }
  }

  // minute
  byte mx1[12] = {
    10,12,13,12,10,7,4,3,2,3,4,7                                                                                                            };
  byte my1[12] = {
    3,4,7,10,12,13,12,10,7,4,3,2                                                                                                            };
  byte mx2[12] = {
    11,12,13,12,11,8,5,3,2,3,5,8                                                                                                            };
  byte my2[12] = {
    3,5,8,11,12,13,12,11,8,5,3,2                                                                                                            };
  for(byte m=5;m<=60;m+=5){
    if(date.minute>=m){
      ledMatrix->paintPixel(minuteColor5, xOff+mx1[m/5-1], +my1[m/5-1]);
      ledMatrix->paintPixel(minuteColor5, xOff+mx2[m/5-1], +my2[m/5-1]);
    }
  }
  if(date.minute%5>=1){
    ledMatrix->paintPixel(minuteColor1, xOff+7, 4);
    ledMatrix->paintPixel(minuteColor1, xOff+8, 4);
  }
  if(date.minute%5>=2){
    ledMatrix->paintPixel(minuteColor1, xOff+11, 7);
    ledMatrix->paintPixel(minuteColor1, xOff+11, 8);
  }
  if(date.minute%5>=3){
    ledMatrix->paintPixel(minuteColor1, xOff+7, 11);
    ledMatrix->paintPixel(minuteColor1, xOff+8, 11);
  }
  if(date.minute%5>=4){
    ledMatrix->paintPixel(minuteColor1, xOff+4, 7);
    ledMatrix->paintPixel(minuteColor1, xOff+4, 8);
  }
  // Second
  if(date.second%5>=1){
    ledMatrix->paintPixel(secondColor, xOff+7, 6);
    ledMatrix->paintPixel(secondColor, xOff+8, 6);
  }
  if(date.second%5>=2){
    ledMatrix->paintPixel(secondColor, xOff+9, 7);
    ledMatrix->paintPixel(secondColor, xOff+9, 8);
  }
  if(date.second%5>=3){
    ledMatrix->paintPixel(secondColor, xOff+7, 9);
    ledMatrix->paintPixel(secondColor, xOff+8, 9);
  }
  if(date.second%5>=4){
    ledMatrix->paintPixel(secondColor, xOff+6, 7);
    ledMatrix->paintPixel(secondColor, xOff+6, 8);
  }
}

// Big digit

void Clock::drawBigDigit(Color color, int x, int y, byte digit){
  switch(digit){
  case 0:
    ledMatrix->line(color, x+2, y+4, x+2, y+9);
    break;
  case 1:
    ledMatrix->line(color, x+2, y, x+2, y+ledMatrix->height-2);
    break;
  case 2:
    ledMatrix->line(color, x, y+4, x+2, y+4);
    ledMatrix->line(color, x+2, y+9, x+4, y+9);
    break;
  case 3:
    ledMatrix->line(color, x, y+4, x+2, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    break;
  case 4:
    ledMatrix->line(color, x+2, y, x+2, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    ledMatrix->line(color, x+2, y+10, x+2, y+13);
    break;
  case 5:
    ledMatrix->line(color, x+2, y+4, x+4, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    break;
  case 6:
    ledMatrix->line(color, x+2, y+4, x+4, y+4);
    ledMatrix->paintPixel(color, x+2, y+9);
    break;
  case 7:
    ledMatrix->line(color, x, y+4, x+2, y+4);
    ledMatrix->line(color, x+2, y+4, x+2, y+13);
    break;
  case 8:
    ledMatrix->paintPixel(color, x+2, y+4);
    ledMatrix->paintPixel(color, x+2, y+9);
    break;
  case 9:
    ledMatrix->paintPixel(color, x+2, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    ledMatrix->line(color, x+2, y+10, x+2, y+13);
    break;
  }
}

void Clock::showBigDigit(){
  ledMatrix->line(RGB(0, 0, value), 0, 0, ledMatrix->width-1, 0);
  ledMatrix->line(RGB(0, 0, value), 0, ledMatrix->height-1, ledMatrix->width-1, ledMatrix->height-1);
  ledMatrix->line(RGB(0, 0, value), 5, 1, 5, ledMatrix->height-2);
  ledMatrix->line(RGB(0, 0, value), 18, 1, 18, ledMatrix->height-2);
  ledMatrix->box(RGB(0, 0, value), ledMatrix->width/2-1, 1, ledMatrix->width/2, ledMatrix->height-2);
  drawBigDigit(RGB(0, 0, value), 0, 1, date.hour/10);
  drawBigDigit(RGB(0, 0, value), 6, 1, date.hour%10);
  drawBigDigit(RGB(0, 0, value), 13, 1, date.minute/10);
  drawBigDigit(RGB(0, 0, value), 19, 1, date.minute%10);
}

// main stuff

Clock::Clock():
Mode(PSTR("Clock")){
  lastLCDUpdate=millis();
  lastLCDUpdate=0;
}

void Clock::loop(){
  unsigned long time;

  // intensity
  value=light->read(MAX_C);
  value+=2;
  if(value>MAX_C) //tunning
    value=MAX_C;
  value=V_GAMMA(value);

  // LCD
  if((time=millis())-lastLCDUpdate>1000){
    lastLCDUpdate=time;
    lcdInfo();
  }

  // LED Matrix
  ledMatrix->fill(BLACK);

  // change mode if MODE button is pressed
  if(button->pressed(BUTTON_A)) {
    EEPROM.write(EEPROM_CLOCK_MODE, EEPROM.read(EEPROM_CLOCK_MODE)+1);
  }

  date=DS1307::getDate();

  switch(EEPROM.read(EEPROM_CLOCK_MODE)){
    // Digital
  case 0:
    showDigital();
    break;
    // Binary
  case 1:
    showBinary();
    break;
    // showDX
  case 2:
    showDX();
    break;
    // Big digit, inspired by http://www.tokyoflash.com/en/watches/kisai/stencil/
  case 3:
    showBigDigit();
    break;
  default:
    EEPROM.write(EEPROM_CLOCK_MODE, 0);
    return;
  }
  ledMatrix->show();
}



















































