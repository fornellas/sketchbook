#ifndef Spectrum_h
#define Spectrum_h

#include <avr/pgmspace.h>

namespace Spectrum {
  PGM_P * enter();
  void loop();
  void exit();
};

#endif


