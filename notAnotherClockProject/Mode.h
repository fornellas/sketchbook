#ifndef Mode_h
#define Mode_h

#include <Arduino.h>

class Mode {
public:
  virtual ~Mode(){};
  // Name
  PGM_P *getName();
  // Thil will be called in a loop
  virtual void loop() = 0;
};

#endif

