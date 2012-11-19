#ifndef Clock_h
#define Clock_h

#include "Mode.h"
#include <SFRGBLEDMatrix.h>
#include <DS1307.h>

#define CLOCK_MODES 3

class Clock: 
public Mode {
private:
  btime_t date;
  byte value; // light intensity
  // LCD
  unsigned long lastLCDUpdate;
  void lcdInfo();
  // Digital
  void showDigital();
  // Binary
  void drawBCDdigit(byte value, int x, int y, byte digit);
  void showBinary();
  // DX Time
  void showDX();
  // Big digit
  void drawBigDigit(Color color, int x, int y, byte digit);
  void showBigDigit();
public:
  Clock();
  void loop();
};

#endif












