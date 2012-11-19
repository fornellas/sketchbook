#include <DS1307.h>

#include "Wire.h"

#define DS1307_ADDRESS 0x68

uint8_t DS1307::bcdToDec(uint8_t val){
  return ( (val/16*10) + (val%16) );
}

uint8_t DS1307::decToBcd(uint8_t val){
  return ( (val/10*16) + (val%10) );
}

btime_t DS1307::readBTimeRTC(){
  int16_t zero=0;
  int8_t oldZone;

  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero);
  Wire.endTransmission();
  Wire.requestFrom(DS1307_ADDRESS, 7);
  bTime.sec = bcdToDec(Wire.read());
  bTime.min = bcdToDec(Wire.read());
  bTime.hour = bcdToDec(Wire.read() & 0b111111);
  bTime.wday = bcdToDec(Wire.read());
  bTime.mday = bcdToDec(Wire.read());
  bTime.mon = bcdToDec(Wire.read());
  bTime.year = 2000+bcdToDec(Wire.read());
  oldZone=bTime.zone;
  bTime.zone = 0; // RTC always UTC
  changeZone(oldZone); // convert to local time
  return bTime;
}

void DS1307::setRTC(btime_t newBTime){
  int16_t zero=0;
  int8_t oldZone;

  // save new time
  bTime=newBTime;
  // save tz
  oldZone=bTime.zone; // save local zone
  // RTC always in UTC
  changeZone(0);
  // set date
  Wire.beginTransmission(DS1307_ADDRESS);
  Wire.write(zero); //stop Oscillator
  Wire.write(decToBcd(bTime.sec));
  Wire.write(decToBcd(bTime.min));
  Wire.write(decToBcd(bTime.hour));
  Wire.write(decToBcd(bTime.wday));
  Wire.write(decToBcd(bTime.mday));
  Wire.write(decToBcd(bTime.mon));
  Wire.write(decToBcd(bTime.year-2000));
  Wire.write(zero); //start 
  Wire.endTransmission();
  // restore tz
  changeZone(oldZone);
}

DS1307::DS1307(int8_t zone){
  bTime.zone=zone;
  readBTimeRTC();
}

