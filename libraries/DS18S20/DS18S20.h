#ifndef DS18S20_H
#define DS18S20_H

#include <inttypes.h>

namespace DS18S20 {
  float read(uint8_t pin, uint8_t *addr);
};

#endif
