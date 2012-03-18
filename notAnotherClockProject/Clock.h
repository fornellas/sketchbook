#ifndef Clock_h
#define Clock_h

#include <avr/pgmspace.h>

namespace Clock {
  PGM_P * enter();
  void loop();
  void exit();
};

#endif


