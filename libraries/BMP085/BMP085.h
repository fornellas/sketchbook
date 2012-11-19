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

#include <Pressure.h>
#include <Temperature.h>
#include <inttypes.h>

#define BMP085_ULTRALOWPOWER 0
#define BMP085_STANDARD      1
#define BMP085_HIGHRES       2
#define BMP085_ULTRAHIGHRES  3

class BMP085:
public Pressure,
public Temperature {
private:
  // Conf
  uint8_t oversampling;
  // I/O
  int16_t bmp085ReadInt(uint8_t address);
  // Temperature
  uint16_t bmp085ReadUT();
  int32_t bmp085ReadB5();
  int16_t bmp085GetTemperature();
  // Pressure
  uint32_t bmp085ReadUP();
  int32_t bmp085GetPressure();
  // Calibration
  struct Calibration{
    int16_t ac1;
    int16_t ac2; 
    int16_t ac3; 
    uint16_t ac4;
    uint16_t ac5;
    uint16_t ac6;
    int16_t b1;
    int16_t b2;
//    int16_t mb;
    int16_t mc;
    int16_t md;
  };
  struct Calibration calibration;
  void bmp085Calibration();
public:
  // Temperature
  double readK();
//  static double readC();
  // Pressure
  uint32_t readPa();
//  static uint32_t readPa(uint8_t os);
  // Oversampling
  void setOversampling(uint8_t newOversampling);
  // Constructor
  BMP085(uint8_t newOversampling);
};

#endif
