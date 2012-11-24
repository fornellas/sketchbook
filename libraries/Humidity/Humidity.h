#ifndef Humidity_h
#define Humidity_h

#include <inttypes.h>

class Humidity{
protected:
  double RH;
  uint8_t pin;
public:
  Humidity();
  // Sensor
  virtual void loadFromSensor(){}; // should be pure virtual, but avr-gcc can not link it...
  // return values
  double getRH();
};

#endif
