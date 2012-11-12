#ifndef Clock_h
#define Clock_h

#include "Mode.h"
#include <SFRGBLEDMatrix.h>
#include <DS1307.h>

#define CLOCK_MODES 3

class Clock: 
public Mode {
private:
  unsigned long lastLCDUpdate;
  void lcdInfo();
  void printOuter12(byte value, int x_offset, int y_offset, Color color);
  void printInner60(byte value, int x_offset, int y_offset, Color colorOuter, Color colorInner);
  void printTimeSmall(struct DS1307::Date date, int x_offset, int y_offset, Color centerColor, Color hourColor, Color minuteColor5, Color minuteColor1, Color secondColor);
  void printTimeBig(struct DS1307::Date date, Color centerColor, Color hourColor, Color minuteColor5, Color minuteColor1, Color secondColor);
  void printDate(struct DS1307::Date date, int x_offset, int y_offset, Color centerColor, Color monthColor, Color dayColor5, Color dayColor1);
  void drawBCDdigit(byte value, int x, int y, byte digit);
  void drawBigDigit(Color color, int x, int y, byte digit);
public:
  Clock();
  void loop();
};

#endif










