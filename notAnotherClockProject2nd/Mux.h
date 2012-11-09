#ifndef Util_h
#define Util_h

#include <inttypes.h>

namespace Mux {
  void begin();
  void setPin(byte addr, uint8_t mode, boolean analog);
  int aRead();
  int aRead(byte addr);
  int dRead();
  int dRead(byte addr);
  void dWrite(uint8_t value);
  void dWrite(uint8_t value, byte addr);
};

#endif



