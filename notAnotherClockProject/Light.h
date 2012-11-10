#ifndef Light_h
#define Light_h

#include <inttypes.h>

#define NUMREADINGS 5

class Light {
private:
  uint8_t pin;
  int readings[NUMREADINGS];
  uint8_t index;
  int total;
public:
  Light(uint8_t pin);
  void update();
  int read(int m);
};

#endif



