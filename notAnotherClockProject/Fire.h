#ifndef Fire_h
#define Fire_h

#include "Mode.h"
#include <Arduino.h>

class Fire: 
public Mode {
public:
  Fire();
  ~Fire();
  PGM_P *getName();
  void loop();
};

#endif
