#ifndef Lamp_h
#define Lamp_h

#include <avr/pgmspace.h>

namespace Lamp {
  PGM_P * enter();
  void loop();
  void exit();
};

#endif


