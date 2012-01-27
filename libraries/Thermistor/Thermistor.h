/*
  Thermistor.h - Temperature reading library for Arduino
  Based on the code found at:
  http://www.arduino.cc/playground/ComponentLib/Thermistor2

  Schematic:

  [Ground] ---- [10k-Resistor] -------|------- [Thermistor] ---- [+5v]
                                      |
                                  Analog Pin
*/

#ifndef Thermistor_h
#define Thermistor_h

#include <inttypes.h>

class Thermistor
{
  private:
    uint8_t pin;
  public:
    Thermistor(uint8_t pin);
    double read();
};

#endif

