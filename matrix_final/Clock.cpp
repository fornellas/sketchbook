#include "Clock.h"
#include <RTCDS1307.h>
#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include <EEPROM.h>
#include "EEPROM.h"
#include "pins.h"

extern RTCDS1307 *rtc;
extern SFRGBLEDMatrix *display;
extern Button *button;

void printOuter12(byte value, int x_offset, int y_offset, Color color) {
  byte x[12] = {
    5,6,6,6,5,3,1,0,0,0,1,3                          };
  byte y[12] = {
    0,1,3,5,6,6,6,5,3,1,0,0                          };
  for(byte h=1;h<13;h++){
    if(value>=h||value==0)
      display->paintPixel(color, x_offset+x[h-1], y_offset+y[h-1]);
  }
}

void printInner60(byte value, int x_offset, int y_offset, Color colorOuter, Color colorInner) {
  byte x[12] = {
    4,5,5,5,4,3,2,1,1,1,2,3                          };
  byte y[12] = {
    1,2,3,4,5,5,5,4,3,2,1,1                          };
  for(byte m=5;m<=60;m+=5){
    if(value>=m)
      display->paintPixel(colorOuter, x_offset+x[m/5-1], y_offset+y[m/5-1]);
  }
  if(value%5>=1)
    display->paintPixel(colorInner, x_offset+3, y_offset+2);
  if(value%5>=2)
    display->paintPixel(colorInner, x_offset+4, y_offset+3);
  if(value%5>=3)
    display->paintPixel(colorInner, x_offset+3, y_offset+4);
  if(value%5>=4)
    display->paintPixel(colorInner, x_offset+2, y_offset+3);
}

void printTimeSmall(Date date, int x_offset, int y_offset, Color centerColor, Color hourColor, Color minuteColor5, Color minuteColor1, Color secondColor) {
  // Middle point
  display->paintPixel(centerColor, x_offset+3, y_offset+3);
  // Hour
  int hour;
  hour=date.hour;
  if(hour>=13)
    hour-=12;
  printOuter12(hour, x_offset, y_offset, hourColor);
  // minute
  printInner60(date.minute, x_offset, y_offset, minuteColor5, minuteColor1);
  // Second
  if(date.second%5>=1)
    display->paintPixel(secondColor, x_offset+4, y_offset+2);
  if(date.second%5>=2)
    display->paintPixel(secondColor, x_offset+4, y_offset+4);
  if(date.second%5>=3)
    display->paintPixel(secondColor, x_offset+2, y_offset+4);
  if(date.second%5>=4)
    display->paintPixel(secondColor, x_offset+2, y_offset+2);
};

void printTimeBig(Date date, Color centerColor, Color hourColor, Color minuteColor5, Color minuteColor1, Color secondColor) {
  // Middle point
  display->paintPixel(centerColor, 7, 7);
  display->paintPixel(centerColor, 8, 7);
  display->paintPixel(centerColor, 7, 8);
  display->paintPixel(centerColor, 8, 8);
  // Hour
  int hour;
  hour=date.hour;
  if(hour>=13)
    hour-=12;
  byte hx1[12] = {
    11,14,15,14,11,7,3,1,0,1,3,7                          };
  byte hy1[12] = {
    1,3,7,11,14,15,14,11,7,3,1,0                          };
  byte hx2[12] = {
    12,14,15,14,12,8,4,1,0,1,4,8                          };
  byte hy2[12] = {
    1,4,8,12,14,15,14,12,8,4,1,0                          };
  for(byte h=1;h<13;h++){
    if(hour>=h||hour==0) {
      display->paintPixel(hourColor, hx1[h-1], hy1[h-1]);
      display->paintPixel(hourColor, hx2[h-1], hy2[h-1]);
    }
  }

  // minute
  byte mx1[12] = {
    10,12,13,12,10,7,4,3,2,3,4,7                          };
  byte my1[12] = {
    3,4,7,10,12,13,12,10,7,4,3,2                          };
  byte mx2[12] = {
    11,12,13,12,11,8,5,3,2,3,5,8                          };
  byte my2[12] = {
    3,5,8,11,12,13,12,11,8,5,3,2                          };
  for(byte m=5;m<=60;m+=5){
    if(date.minute>=m){
      display->paintPixel(minuteColor5, mx1[m/5-1], +my1[m/5-1]);
      display->paintPixel(minuteColor5, mx2[m/5-1], +my2[m/5-1]);
    }
  }
  if(date.minute%5>=1){
    display->paintPixel(minuteColor1, 7, 4);
    display->paintPixel(minuteColor1, 8, 4);
  }
  if(date.minute%5>=2){
    display->paintPixel(minuteColor1, 11, 7);
    display->paintPixel(minuteColor1, 11, 8);
  }
  if(date.minute%5>=3){
    display->paintPixel(minuteColor1, 7, 11);
    display->paintPixel(minuteColor1, 8, 11);
  }
  if(date.minute%5>=4){
    display->paintPixel(minuteColor1, 4, 7);
    display->paintPixel(minuteColor1, 4, 8);
  }
  // Second
  if(date.second%5>=1){
    display->paintPixel(secondColor, 7, 6);
    display->paintPixel(secondColor, 8, 6);
  }
  if(date.second%5>=2){
    display->paintPixel(secondColor, 9, 7);
    display->paintPixel(secondColor, 9, 8);
  }
  if(date.second%5>=3){
    display->paintPixel(secondColor, 7, 9);
    display->paintPixel(secondColor, 8, 9);
  }
  if(date.second%5>=4){
    display->paintPixel(secondColor, 6, 7);
    display->paintPixel(secondColor, 6, 8);
  }
}

void printDate(Date date, int x_offset, int y_offset, Color centerColor, Color monthColor, Color dayColor5, Color dayColor1) {
  // Middle point
  display->paintPixel(centerColor, x_offset+3, y_offset+3);
  // Month
  printOuter12(date.month, x_offset, y_offset, monthColor);
  // Day
  printInner60(date.monthDay, x_offset, y_offset, dayColor5, dayColor1);
};

void Clock::enter(){
  display->gamma(false); 
}

void Clock::loop() {
  struct Date date;
  int light;
  byte value;

  display->fill(BLACK);

  // change mode if MODE button is pressed
  if(button->pressed(BUTTON_A)) {
    currMode++;
    if(currMode>=CLOCK_MODES)
      currMode=0;
    EEPROM.write(EEPROM_CLOCK_MODE, currMode);
  }

  // FIXME average reading
  pinMode(PIN_PHOTORESISTOR, INPUT);
  digitalWrite(PIN_CD74HC4067_S0, ADDR_PHOTORESISTOR & 0x1);
  digitalWrite(PIN_CD74HC4067_S1, ADDR_PHOTORESISTOR & 0x2);
  digitalWrite(PIN_CD74HC4067_S2, ADDR_PHOTORESISTOR & 0x4);
  digitalWrite(PIN_CD74HC4067_S3, ADDR_PHOTORESISTOR & 0x8);
  light=0;
  for(byte i=0;i<10;i++)
    light+=analogRead(PIN_PHOTORESISTOR);
  light/=10;

  value=light*7/1023;

  date=rtc->getDate();

  switch(currMode){
  case 0:
    int xOffset;
    int yOffset;
    xOffset=(display->width>>1)-8;
    yOffset=(display->height>>1)-4;
    xOffset=0;
    yOffset=3;
    display->print(RGB(value,0,0), xOffset+0, yOffset+0, 5, 48+(date.hour/10));
    display->print(RGB(value,0,0), xOffset+4, yOffset+0, 5, 48+(date.hour%10));
    display->print(RGB(0,value,0), xOffset+8, yOffset+0, 5, 48+(date.minute/10));
    display->print(RGB(0,value,0), xOffset+12, yOffset+0, 5, 48+(date.minute%10));
    display->print(RGB(0,0,value), xOffset+4, yOffset+6, 5, 48+(date.second/10));
    display->print(RGB(0,0,value), xOffset+8, yOffset+6, 5, 48+(date.second%10));
    break;
  case 1:
    printTimeSmall(date, 0, 0, RGB(value,value,value), RGB(value,0,0), RGB(value,value,0), RGB(0,value,0), RGB(0,0,value));
    printDate(date, 8, 0, RGB(value,value,value), RGB(value,0,0), RGB(value,value,0), RGB(0,value,0));
    break;
  case 2:
    printTimeBig(date, RGB(value,value,value), RGB(value,0,0), RGB(value,value,0), RGB(0,value,0), RGB(0,0,value));
    break;
  }
  display->show();
}
