#include "Clock.h"

#include <DS1307.h>
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include <EEPROM.h>
#include "EEPROM.h"
#include "pins.h"

#include <Arduino.h>

void SerialPrintPGM (PGM_P s);

extern SFRGBLEDMatrix *display;
extern Button *button;

PGM_P * Clock::enter(){
  SerialPrintPGM(PSTR("Clock::enter()\n"));
  return NULL;
}

void Clock::loop(){
  struct DS1307::Date date;
  byte value=15;

  display->fill(BLACK);

  date=DS1307::getDate();

  switch(0){
  case 0:
    int xOffset;
    int yOffset;
    xOffset=(display->width>>1)-8;
    yOffset=(display->height>>1)-6;
    display->print(RGB(value,0,0), xOffset+0, yOffset+0, 5, 48+(date.hour/10));
    display->print(RGB(value,0,0), xOffset+4, yOffset+0, 5, 48+(date.hour%10));
    display->print(RGB(0,value,0), xOffset+8, yOffset+0, 5, 48+(date.minute/10));
    display->print(RGB(0,value,0), xOffset+12, yOffset+0, 5, 48+(date.minute%10));
    display->print(RGB(0,0,value), xOffset+4, yOffset+7, 5, 48+(date.second/10));
    display->print(RGB(0,0,value), xOffset+8, yOffset+7, 5, 48+(date.second%10));
    break;
  }
  display->show();
}

void Clock::exit(){
  SerialPrintPGM(PSTR("Clock::exit()\n"));
}











