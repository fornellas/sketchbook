#include <Wire.h>
#include <Time.h>
#include <DS1307.h>

#define TIMEZONE -3

DS1307 *time;

void setup(){
  btime_t bt;

  Serial.begin(115200);
  Wire.begin();
  time=new DS1307(TIMEZONE);

  bt.sec=50;
  bt.min=21;
  bt.hour=21;
  bt.wday=MONDAY;
  bt.mday=19;
  bt.mon=NOVEMBER;
  bt.year=2012;
  bt.zone=TIMEZONE;

//  time->setRTC(bt);
}

void loop(){
  btime_t bt;

  bt=time->readBTimeRTC();
  Serial.print("TZ(");
  Serial.print(bt.zone);
  Serial.print(") ");
  Serial.print(bt.mday);
  Serial.print("/");
  Serial.print(bt.mon);
  Serial.print("/");
  Serial.print(bt.year);
  Serial.print(" ");
  Serial.print(bt.wday);
  Serial.print(" ");
  Serial.print(bt.hour);
  Serial.print(":");
  Serial.print(bt.min);
  Serial.print(":");
  Serial.println(bt.sec);
  delay(1000);
}
