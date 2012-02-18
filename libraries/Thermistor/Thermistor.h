/*
  Thermistor.h - Temperature reading library for Arduino

  [Ground] ---- [10k Resistor] -------|------- [ Thermistor] ---- [Vcc]
                                      |
                                  Analog Pin

  Constants configured for Vishay part NTCLE100E3103JB0:
  http://www.sparkfun.com/products/250
  adjust at Thermistor.cpp if needed.
*/



#ifndef Thermistor_h
#define Thermistor_h

#include <inttypes.h>

namespace Thermistor
{
  double read(uint8_t pin);
};

#endif

