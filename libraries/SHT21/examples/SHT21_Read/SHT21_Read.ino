#include <SHT21.h>

SHT21 sht21;

void error(SHT21Error e) {
  Serial.print("Error: ");
  Serial.println(e);
  while(true);
}

void setup(){
  SHT21Error e;
  
  // Serial.begin(74880);
  Serial.begin(115200);
  Serial.println(__FILE__);

#if defined(ESP8266) || defined(ESP32)
  Wire.begin(
    4, // D2
    5  // D1
  );
#else
  Wire.begin();
#endif

  if((e=sht21.begin()))
    error(e);
}


void loop(){
  SHT21Error e;
  double temperature, rh;

  if((e=sht21.GetTemperature(&temperature))) {
    error(e);
  }
  Serial.print(temperature, 2);
  Serial.print(" ");

  if((e=sht21.GetRelativeHumidity(&rh))) {
    error(e);
  }
  Serial.print(rh, 2);
  Serial.println();
}