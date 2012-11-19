#include <Wire.h>
#include <Pressure.h>
#include <BMP085.h>

BMP085 *sensor;

void setup(){
  Serial.begin(115200);
  Wire.begin();
  sensor=new BMP085(BMP085_ULTRAHIGHRES);
}

void loop(){
  Serial.print(sensor->readC());
  Serial.println(" C");
  Serial.print(sensor->readPa());
  Serial.println(" Pa");
  Serial.println("");
  delay(1000);
}
