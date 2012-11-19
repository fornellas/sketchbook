#ifndef DS18S20_H
#define DS18S20_H

#include <Temperature.h>
#include <inttypes.h>
#include <OneWire.h>

class DS18S20:
public Temperature {
  uint8_t *addr;
  OneWire *ow;
public:
  double readK();
  DS18S20(uint8_t *addr, OneWire *ow);
};

#endif
