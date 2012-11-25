#ifndef Pressure_h
#define Pressure_h

#include <inttypes.h>

#define AVERAGE_SEA_LEVEL_PRESSURE 101325

class Pressure{
protected:
  double Pa;
public:
  Pressure();
  // Sensor
  virtual void loadFromSensor(){}; // should be pure virtual, but avr-gcc can not link it...
  // return values
  double getPa(); // Pascal
  double getInHg(); // Inches Hg
  double getHPa(); // hPa / millibar
  double getAtm(); // Atmosphere
  // Altitude
  double altitude(double seaLevelPa);
  static double expectedPressure(double seaLevelPa, double meters);
};

#endif
