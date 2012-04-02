/* 
  BMP085 Pressure / Temperature sensor library

  Heavily based on code from:

    BMP085 Extended Example Code
    http://www.sparkfun.com/tutorial/Barometric/BMP085_Example_Code.pde
    by: Jim Lindblom
    SparkFun Electronics
    date: 1/18/11
    license: CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/
*/

#ifndef BMP085_H
#define BMP085_H

#include <Wire.h>

namespace BMP085 {
  struct Calibration{
    int ac1;
    int ac2; 
    int ac3; 
    unsigned int ac4;
    unsigned int ac5;
    unsigned int ac6;
    int b1;
    int b2;
    int mb;
    int mc;
    int md;
  };
  // Calibration data
  struct BMP085::Calibration calibration();
  // Temperature in C
  float readTemperature();
  float readTemperature(struct BMP085::Calibration *c);
  // Pressure in Pa
  long readPressure();
  long readPressure(struct BMP085::Calibration *c);
};

#endif
