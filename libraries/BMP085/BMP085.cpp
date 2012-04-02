#include <BMP085.h>
#include <Arduino.h>

#define BMP085_ADDRESS 0x77  // I2C address of BMP085
#define OSS 0 // Oversampling Setting

//
// I/O
//

// Read 2 bytes from the BMP085
// First byte will be from 'address'
// Second byte will be from 'address'+1
int bmp085ReadInt(unsigned char address)
{
  unsigned char msb, lsb;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  
  return (int) msb<<8 | lsb;
}

//
// Temperature
//

// Read the uncompensated temperature value
unsigned int bmp085ReadUT()
{
  unsigned int ut;
  
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

long bmp085ReadB5(struct BMP085::Calibration *c){
  long x1, x2;
  
  x1 = (((long)bmp085ReadUT() - (long)c->ac6)*(long)c->ac5) >> 15;
  x2 = ((long)c->mc << 11)/(x1 + c->md);
  return x1 + x2;
}

// Calculate temperature given ut.
// Value returned will be in units of 0.1 deg C
short bmp085GetTemperature(struct BMP085::Calibration *c){
  return ((bmp085ReadB5(c) + 8)>>4);  
}

//
// Pressure
//

// Read the uncompensated pressure value
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Write 0x34+(OSS<<6) into register 0xF4
  // Request a pressure reading w/ oversampling setting
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Wait for conversion, delay time dependent on OSS
  delay(2 + (3<<OSS));
  
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
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}

// Calculate pressure given up
// calibration values must be known
// b5 is also required so bmp085GetTemperature(...) must be called first.
// Value returned will be pressure in units of Pa.
long bmp085GetPressure(struct BMP085::Calibration *c)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = bmp085ReadB5(c) - 4000;
  // Calculate B3
  x1 = (c->b2 * (b6 * b6)>>12)>>11;
  x2 = (c->ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)c->ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (c->ac3 * b6)>>13;
  x2 = (c->b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (c->ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(bmp085ReadUP() - b3) * (50000>>OSS));
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
void bmp085Calibration(struct BMP085::Calibration *c)
{
  c->ac1 = bmp085ReadInt(0xAA);
  c->ac2 = bmp085ReadInt(0xAC);
  c->ac3 = bmp085ReadInt(0xAE);
  c->ac4 = bmp085ReadInt(0xB0);
  c->ac5 = bmp085ReadInt(0xB2);
  c->ac6 = bmp085ReadInt(0xB4);
  c->b1 = bmp085ReadInt(0xB6);
  c->b2 = bmp085ReadInt(0xB8);
  c->mb = bmp085ReadInt(0xBA);
  c->mc = bmp085ReadInt(0xBC);
  c->md = bmp085ReadInt(0xBE);
}

//
// Library
//

struct BMP085::Calibration BMP085::calibration(){
  struct BMP085::Calibration c;
  bmp085Calibration(&c);
  return c;
}

float BMP085::readTemperature(){
  struct BMP085::Calibration c;
  c=BMP085::calibration();
  return (float)bmp085GetTemperature(&c)/10.0;
}

float BMP085::readTemperature(struct BMP085::Calibration *c){
  return (float)bmp085GetTemperature(c)/10.0;
}

long BMP085::readPressure(){
  struct BMP085::Calibration c;
  c=BMP085::calibration();
  return bmp085GetPressure(&c);
}

long BMP085::readPressure(struct BMP085::Calibration *c){
  return bmp085GetPressure(c);;
}
