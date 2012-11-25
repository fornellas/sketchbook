#ifndef Fire_h
#define Fire_h

#include "Mode.h"

#include <SFRGBLEDMatrix.h>

class Fire: 
public Mode {
private:
  Color incandescent(byte p);
  byte *pixmap;
public:
  Fire();
  inline void loop();
  ~Fire();
};

#endif












