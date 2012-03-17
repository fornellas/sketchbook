#ifndef Lamp_h
#define Lamp_h

#include "Mode.h"
#include <Arduino.h>

class Lamp: 
public Mode {
public:
  Lamp();
  ~Lamp();
  PGM_P *getName();
  void loop();
};

#endif
