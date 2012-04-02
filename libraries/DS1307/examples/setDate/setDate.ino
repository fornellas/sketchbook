#include <DS1307.h>
#include <Wire.h>

void setup(){
  struct DS1307::Date d;

  Serial.begin(115200);
  Wire.begin();

  d.second=50;
  d.minute=18;
  d.hour=0;
  d.weekDay=TUESDAY;
  d.monthDay=7;
  d.month=FEBRUARY;
  d.year=2012;
  DS1307::setDate(d);
}

void loop(){
  struct DS1307::Date d;

  d=DS1307::getDate();
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
