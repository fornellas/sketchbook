#ifndef Clock_h
#define Clock_h

#include "Mode.h"

class Clock: 
public Mode {
public:
  Clock() : 
  Mode("Clock"){};
  void loop();
};

#endif



