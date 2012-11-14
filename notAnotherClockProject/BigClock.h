#ifndef BigClock_h
#define BigClock_h

#include "Mode.h"
#include <SFRGBLEDMatrix.h>
#include <DS1307.h>

class BigClock: 
public Mode {
private:
public:
  BigClock();
  void loop();
};

#endif
