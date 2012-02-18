#include <RTCDS1307.h>
#include "Wire.h"

RTCDS1307 *rtc;

void setup(){
  Serial.begin(115200);
  rtc=new RTCDS1307();
  Wire.begin();
}

void loop(){
  struct Date d;

  d.second=50;
  d.minute=18;
  d.hour=0;
  d.weekDay=TUESDAY;
  d.monthDay=7;
  d.month=FEBRUARY;
  d.year=2012;
//  rtc->setDate(d);
  while(1){
    d=rtc->getDate();
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







