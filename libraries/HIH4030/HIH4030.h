#ifndef HIH4030_h
#define HIH4030_h

#include <Arduino.h>

namespace HIH4030 {
  void setup(uint8_t pin); // 5V Arduinos only!
  float read(uint8_t pin, float temperature); // Call setup() before
  float calculate(float VoutVdc, float temperature); // VoutVdc = Vout / Vdc. Eg: float(analogRead(pin))/1023.0
};

#endif
