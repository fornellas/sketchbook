#include <RTCDS1307.h>
#include "Wire.h"

void setup(){
  Serial.begin(115200);
}

void loop(){
  RTCDS1307 rtc;
  struct date d;
  d.second=50;
  d.minute=29;
  d.hour=18;
  d.weekDay=7;
  d.monthDay=14;
  d.month=1;
  d.year=2012;
  rtc.setDate(d);
  while(1){
    d=rtc.getDate();
    Serial.print(d.monthDay);
    Serial.print("/");
    Serial.print(d.month);
    Serial.print("/");
    Serial.print(d.year);
    Serial.print(" ");
    Serial.print(d.weekDay);
    Serial.print(" ");
    Serial.print(d.hour);
    Serial.print(":");
    Serial.print(d.minute);
    Serial.print(":");
    Serial.println(d.second);
    delay(1000);
  }
}







