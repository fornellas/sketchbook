#include <Humidity.h>
#include <Pressure.h>
#include <Temperature.h>
#include <HIH4030.h>
#include <Wire.h>
#include <BMP085.h>

// Analog pin
#define PIN_HIH4030 A2

HIH4030 *humidity;
BMP085 *bmp085;

void setup(){
  Serial.begin(115200);
  Wire.begin();
  bmp085=new BMP085(BMP085_ULTRAHIGHRES);
  humidity=new HIH4030(PIN_HIH4030, bmp085);
}

void loop(){
  humidity->loadFromSensor();
  Serial.print(humidity->getRH());
  Serial.println("%");

  delay(1000); 
}
