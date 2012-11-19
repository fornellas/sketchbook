#ifndef Pressure_h
#define Pressure_h

#include <inttypes.h>

#define AVERAGE_SEA_LEVEL_PRESSURE 101325

class Pressure{
public:
  // Pascal
  virtual uint32_t readPa()=0;
  // inches Hg
  virtual double readInHg();
  // hPa
  virtual double readHPa();
  // Atmosphere
  virtual double readAtm();
  // Altitude
  static double calcAltitude(uint32_t seaLevelPa, uint32_t Pa);
  double readCalcAltitude(uint32_t seaLevelPa);
};

#endif
