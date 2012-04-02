#include <DS18S20.h>
#include <Wire.h>
#include <OneWire.h>

void setup(){
  Serial.begin(115200);
}

void loop(){
  byte addr[]={40, 200, 10, 228, 3, 0, 0, 62};
  
  Serial.println(DS18S20::read(2, addr));
}

