#ifndef Temperature_h
#define Temperature_h

#include <inttypes.h>

class Temperature{
protected:
  double K;
public:
  Temperature();
  // Sensor
  virtual void loadFromSensor(){}; // should be pure virtual, but avr-gcc can not link it...
  // return values
  double getK(); // Kelvin
  double getC(); // Celsius
  double getF(); // Fahrenheit
  // conversion
  static double convC2F(double C);
  static double convF2C(double F);
};

#endif
