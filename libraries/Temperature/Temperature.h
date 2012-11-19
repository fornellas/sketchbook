#ifndef Temperature_h
#define Temperature_h

#include <inttypes.h>

class Temperature{
public:
  // Kelvin
  virtual double readK()=0;
  // Celsius
  virtual double readC();
  // Fahrenheit
  virtual double readF();
  // Conversion
  static double convC2F(double C);
  static double convC2K(double C);
};

#endif
