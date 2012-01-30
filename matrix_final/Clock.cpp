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

void printTime(Date date, int x_offset, int y_offset, byte hourColor, byte minuteColor5, byte minuteColor1, byte secondColor) {
  // Hour
  if(date.hour>=1||date.hour==0)
    display->paintPixel(hourColor, x_offset+5, y_offset+0);
  if(date.hour>=2||date.hour==0)
    display->paintPixel(hourColor, x_offset+6, y_offset+1);
  if(date.hour>=3||date.hour==0)
    display->paintPixel(hourColor, x_offset+6, y_offset+3);
  if(date.hour>=4||date.hour==0)
    display->paintPixel(hourColor, x_offset+6, y_offset+5);
  if(date.hour>=5||date.hour==0)
    display->paintPixel(hourColor, x_offset+5, y_offset+6);
  if(date.hour>=6||date.hour==0)
    display->paintPixel(hourColor, x_offset+3, y_offset+6);
  if(date.hour>=7||date.hour==0)
    display->paintPixel(hourColor, x_offset+1, y_offset+6);
  if(date.hour>=8||date.hour==0)
    display->paintPixel(hourColor, x_offset+0, y_offset+5);
  if(date.hour>=9||date.hour==0)
    display->paintPixel(hourColor, x_offset+0, y_offset+3);
  if(date.hour>=10||date.hour==0)
    display->paintPixel(hourColor, x_offset+0, y_offset+1);
  if(date.hour>=11||date.hour==0)
    display->paintPixel(hourColor, x_offset+1, y_offset+0);
  if(date.hour>=12||date.hour==0)
    display->paintPixel(hourColor, x_offset+3, y_offset+0);
  // minute 5
  if(date.minute>=5)
    display->paintPixel(minuteColor5, x_offset+4, y_offset+1);
  if(date.minute>=10)
    display->paintPixel(minuteColor5, x_offset+5, y_offset+2);
  if(date.minute>=15)
    display->paintPixel(minuteColor5, x_offset+5, y_offset+3);
  if(date.minute>=20)
    display->paintPixel(minuteColor5, x_offset+5, y_offset+4);
  if(date.minute>=25)
    display->paintPixel(minuteColor5, x_offset+4, y_offset+5);
  if(date.minute>=30)
    display->paintPixel(minuteColor5, x_offset+3, y_offset+5);
  if(date.minute>=35)
    display->paintPixel(minuteColor5, x_offset+2, y_offset+5);
  if(date.minute>=40)
    display->paintPixel(minuteColor5, x_offset+1, y_offset+4);
  if(date.minute>=45)
    display->paintPixel(minuteColor5, x_offset+1, y_offset+3);
  if(date.minute>=50)
    display->paintPixel(minuteColor5, x_offset+1, y_offset+2);
  if(date.minute>=55)
    display->paintPixel(minuteColor5, x_offset+2, y_offset+1);
  if(date.minute>=60)
    display->paintPixel(minuteColor5, x_offset+3, y_offset+1);
  // minute 1
  if(date.minute%5>=1)
    display->paintPixel(minuteColor1, x_offset+3, y_offset+2);
  if(date.minute%5>=2)
    display->paintPixel(minuteColor1, x_offset+4, y_offset+3);
  if(date.minute%5>=3)
    display->paintPixel(minuteColor1, x_offset+3, y_offset+4);
  if(date.minute%5>=4)
    display->paintPixel(minuteColor1, x_offset+2, y_offset+3);
  // Second
  if(date.second%5>=0)
    display->paintPixel(WHITE, x_offset+3, y_offset+3);
  if(date.second%5>=1)
    display->paintPixel(secondColor, x_offset+4, y_offset+2);
  if(date.second%5>=2)
    display->paintPixel(secondColor, x_offset+4, y_offset+4);
  if(date.second%5>=3)
    display->paintPixel(secondColor, x_offset+2, y_offset+4);
  if(date.second%5>=4)
    display->paintPixel(secondColor, x_offset+2, y_offset+2);
};

void Clock::loop() {
  struct Date date;
  static boolean lastA;

  display->fill(BLACK);

  // change mode if MODE button is pressed
  if(button->state(A)&&lastA==false) {
    currMode++;
    if(currMode>=CLOCK_MODES)
      currMode=0;
    EEPROM.write(EEPROM_CLOCK_MODE, currMode);
  }
  lastA=button->state(A);

  date=rtc->getDate();

  switch(currMode){
  case 0:
    display->printChar4p(char(48+(date.hour/10)), RED, 0, 0);
    display->printChar4p(char(48+(date.hour%10)), RED, 4, 0);
    display->printChar4p(char(48+(date.minute/10)), BLUE, 8, 0);
    display->printChar4p(char(48+(date.minute%10)), BLUE, 12, 0);
    display->printChar4p(char(48+(date.second/10)), GREEN, 4, 4);
    display->printChar4p(char(48+(date.second%10)), GREEN, 8, 4);
    break;
  case 1:
    printTime(date, 4, 0, RED, YELLOW, GREEN, BLUE);
    break;
  }
  display->show();
}
