#ifndef Equalizer_h
#define Equalizer_h

#include "Mode.h"

class Equalizer: 
public Mode {
private:
  int filter;
  uint8_t pin;
public:
  Equalizer();
  void loop();
};

#endif










