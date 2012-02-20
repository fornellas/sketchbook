#ifndef Fire_h
#define Fire_h

#include "Mode.h"

class Fire: 
public Mode {
private:
  byte *pixmap;
public:
  Fire() : 
  Mode("Fire"){};
  void loop();
  void enter();
  void exit();
};

#endif

