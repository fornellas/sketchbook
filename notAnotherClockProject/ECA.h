#ifndef ECA_h
#define ECA_h

#include <SFRGBLEDMatrix.h>

#include "Mode.h"

class ECA: 
public Mode {
private:
  byte rule;
  byte row;
  Color color;
  void newAutomata();
  byte getPattern(int x, int y);
  unsigned long lastLCDUpdate;
public:
  ECA();
  inline void loop();
  ~ECA();
};

#endif




