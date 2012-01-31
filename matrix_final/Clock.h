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
public:
  Clock() : 
  Mode("Clock"){
    currMode=EEPROM.read(EEPROM_CLOCK_MODE);
    if(currMode>=CLOCK_MODES)
      currMode=0;
  };
  void loop();
};

#endif





