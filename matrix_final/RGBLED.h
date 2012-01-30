#ifndef RGBLED_h
#define RGBLED_h

#include "Mode.h"

class RGBLED: 
public Mode {
public:
  RGBLED() : 
  Mode("RGB LED"){  
  };
  void loop();
  void exit();
};

#endif

