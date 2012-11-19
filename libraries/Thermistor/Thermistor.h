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
#include <Temperature.h>

class Thermistor:
public Temperature{
private:
  uint8_t pin;
public
  double readK();
  Thermistor(uint8_t pin);
};

#endif

