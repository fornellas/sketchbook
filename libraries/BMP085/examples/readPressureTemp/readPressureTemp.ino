#include <Wire.h>

#include <BMP085.h>

void setup(){
  Serial.begin(115200);
  Wire.begin();
}

void loop(){
  Serial.print(BMP085::readTemperature());
  Serial.println(" C");
  Serial.print(BMP085::readPressure());
  Serial.println(" Pa");
  Serial.println("");
  delay(1000);
}
