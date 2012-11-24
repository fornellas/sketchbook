#include <BMP085.h>
#include <Wire.h>
#include <Arduino.h>

#define BMP085_ADDRESS 0x77  // I2C address of BMP085

//
// I/O
//

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int16_t BMP085::bmp085ReadInt(uint8_t address){
  uint8_t msb, lsb;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  
  return (int16_t) msb<<8 | lsb;
}

//
// Temperature
//

// Read the uncompensated temperature value
uint16_t BMP085::bmp085ReadUT(){
  uint16_t ut;
  
  // Write 0x2E into Register 0xF4
  // This requests a temperature reading
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Wait at least 4.5ms
  delay(5);
  
  // Read two bytes from registers 0xF6 and 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

int32_t BMP085::bmp085ReadB5(){
  int32_t x1, x2;
  
  x1 = (((int32_t)bmp085ReadUT() - (int32_t)calibration.ac6)*(int32_t)calibration.ac5) >> 15;
  x2 = ((int32_t)calibration.mc << 11)/(x1 + calibration.md);
  return x1 + x2;
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
int16_t BMP085::bmp085GetTemperature(){
  return ((bmp085ReadB5() + 8)>>4);  
}

//
// Pressure
//

// Read the uncompensated pressure value
uint32_t BMP085::bmp085ReadUP(){
  uint8_t msb, lsb, xlsb;
  uint32_t up = 0;
  
  // Write 0x34+(oversampling<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (oversampling<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on oversampling
  delay(2 + (3<<oversampling));
  
  // Read register 0xF6 (MSB), 0xF7 (LSB), and 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Wait for data to become available
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((uint32_t) msb << 16) | ((uint32_t) lsb << 8) | (uint32_t) xlsb) >> (8-oversampling);
  
  return up;
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
int32_t BMP085::bmp085GetPressure(){
  int32_t x1, x2, x3, b3, b6, p;
  uint32_t b4, b7;
  
  b6 = bmp085ReadB5() - 4000;
  // Calculate B3
  x1 = (calibration.b2 * (b6 * b6)>>12)>>11;
  x2 = (calibration.ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((int32_t)calibration.ac1)*4 + x3)<<oversampling) + 2)>>2;
  
  // Calculate B4
  x1 = (calibration.ac3 * b6)>>13;
  x2 = (calibration.b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (calibration.ac4 * (uint32_t)(x3 + 32768))>>15;
  
  b7 = ((uint32_t)(bmp085ReadUP() - b3) * (50000>>oversampling));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}


//
// Calibration
//

// Stores all of the bmp085's calibration values into global variables
// Calibration values are required to calculate temp and pressure
// This function should be called at the beginning of the program
void BMP085::bmp085Calibration(){
  calibration.ac1 = bmp085ReadInt(0xAA);
  calibration.ac2 = bmp085ReadInt(0xAC);
  calibration.ac3 = bmp085ReadInt(0xAE);
  calibration.ac4 = bmp085ReadInt(0xB0);
  calibration.ac5 = bmp085ReadInt(0xB2);
  calibration.ac6 = bmp085ReadInt(0xB4);
  calibration.b1 = bmp085ReadInt(0xB6);
  calibration.b2 = bmp085ReadInt(0xB8);
//  calibration.mb = bmp085ReadInt(0xBA);
  calibration.mc = bmp085ReadInt(0xBC);
  calibration.md = bmp085ReadInt(0xBE);
}

//
// Public
//

// Temperature
void BMP085::loadFromSensor(){
  K=((double)bmp085GetTemperature()/10.0)+273.15;
}

// Pressure
uint32_t BMP085::readPa(){
  return bmp085GetPressure();
}

// Oversampling
void BMP085::setOversampling(uint8_t newOversampling){
  oversampling=newOversampling;
}

// Constructor
BMP085::BMP085(uint8_t newOversampling){
  oversampling=newOversampling;
  bmp085Calibration();
}
