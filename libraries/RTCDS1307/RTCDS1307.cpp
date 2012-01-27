#include <RTCDS1307.h>

#include <Arduino.h>
// Must be included in main project file too
#include "Wire.h"

#define DS1307_ADDRESS 0x68

RTCDS1307::RTCDS1307(bool w) {
  if(w)
    Wire.begin();
};

byte RTCDS1307::bcdToDec(byte val)  {
  return ( (val/16*10) + (val%16) );
}

struct date RTCDS1307::getDate() {
  int zero=0;
  struct date d;
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  d.second = bcdToDec(Wire.read());
  d.minute = bcdToDec(Wire.read());
  d.hour = bcdToDec(Wire.read() & 0b111111);
  d.weekDay = bcdToDec(Wire.read());
  d.monthDay = bcdToDec(Wire.read());
  d.month = bcdToDec(Wire.read());
  d.year = 2000+bcdToDec(Wire.read());
  return d;
}

byte RTCDS1307::decToBcd(byte val){
// Convert normal decimal numbers to binary coded decimal
  return ( (val/10*16) + (val%10) );
}

void RTCDS1307::setDate(struct date d) {
  int zero=0;
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator
  Wire.write(decToBcd(d.second));
  Wire.write(decToBcd(d.minute));
  Wire.write(decToBcd(d.hour));
  Wire.write(decToBcd(d.weekDay));
  Wire.write(decToBcd(d.monthDay));
  Wire.write(decToBcd(d.month));
  Wire.write(decToBcd(d.year-2000));
  Wire.write(zero); //start 
  Wire.endTransmission();
}
