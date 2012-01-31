#include "Clock.h"
#include <RTCDS1307.h>
#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include <EEPROM.h>
#include "EEPROM.h"

extern RTCDS1307 *rtc;
extern Thermistor *thermistor;
extern SFRGBLEDMatrix *display;
extern Button *button;

void printOuter12(byte value, int x_offset, int y_offset, byte color) {
  byte x[12] = {
    5,6,6,6,5,3,1,0,0,0,1,3        };
  byte y[12] = {
    0,1,3,5,6,6,6,5,3,1,0,0        };
  for(byte h=1;h<13;h++){
    if(value>=h||value==0)
      display->paintPixel(color, x_offset+x[h-1], y_offset+y[h-1]);
  }
}

void printInner60(byte value, int x_offset, int y_offset, byte colorOuter, byte colorInner) {
  byte x[12] = {
    4,5,5,5,4,3,2,1,1,1,2,3        };
  byte y[12] = {
    1,2,3,4,5,5,5,4,3,2,1,1        };
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

void printTime(Date date, int x_offset, int y_offset, byte hourColor, byte minuteColor5, byte minuteColor1, byte secondColor) {
  // Middle point
  display->paintPixel(WHITE, x_offset+3, y_offset+3);
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

void printDate(Date date, int x_offset, int y_offset, byte monthColor, byte dayColor5, byte dayColor1) {
  // Middle point
  display->paintPixel(WHITE, x_offset+3, y_offset+3);
  // Month
  printOuter12(date.month, x_offset, y_offset, monthColor);
  // Day
  printInner60(date.monthDay, x_offset, y_offset, dayColor5, dayColor1);
};

void Clock::loop() {
  struct Date date;

  display->fill(BLACK);

  // change mode if MODE button is pressed
  if(button->pressed(A)) {
    currMode++;
    if(currMode>=CLOCK_MODES)
      currMode=0;
    EEPROM.write(EEPROM_CLOCK_MODE, currMode);
  }

  date=rtc->getDate();

  switch(currMode){
  case 0:
    display->printChar4p(char(48+(date.hour/10)), RED, 0, 0);
    display->printChar4p(char(48+(date.hour%10)), RED, 4, 0);
    display->printChar4p(char(48+(date.minute/10)), GREEN, 8, 0);
    display->printChar4p(char(48+(date.minute%10)), GREEN, 12, 0);
    display->printChar4p(char(48+(date.second/10)), BLUE, 4, 4);
    display->printChar4p(char(48+(date.second%10)), BLUE, 8, 4);
    break;
  case 1:
    printTime(date, 4, 0, RED, YELLOW, GREEN, BLUE);
    break;
  case 2:
    printTime(date, 0, 0, RED, YELLOW, GREEN, BLUE);
    printDate(date, 8, 0, RED, YELLOW, GREEN);
  }
  display->show();
}







