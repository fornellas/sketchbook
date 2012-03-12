#ifndef Clock_h
#define Clock_h

#include "Mode.h"
#include <EEPROM.h>
#include "EEPROM.h"

#define CLOCK_MODES 3

class Clock: 
public Mode {
private:
  byte currMode;
};

#endif
