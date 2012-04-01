/*
  Sample code for Honeywell's HIH4030 humidity sensor.
  */

#include <HIH4030.h>

// Analog pin
#define PIN_HIH4030 A3
// Sensor is temperature dependent, ideally you should read it with another sensor
#define TEMPERATURE 26.0

void setup(){
  Serial.begin(115200);
  // This works only with 5V Arduinos
  // setup() must be called before read()
  HIH4030::setup(PIN_HIH4030);
}

void loop(){
  float humidity;
  float VoutVdc;

  //
  // Mode #1
  //

  // read ADC value and return humidity
  // call setup() before calling read()
  humidity=HIH4030::read(PIN_HIH4030, TEMPERATURE);
  Serial.print(humidity);
  Serial.println("%");

  //
  // Mode #2
  //

  // calculate() can be used if you read the voltage yourself
  // no need to call setup() before calculate()

  // Vout / Vdc
  // Vref must be equal Vdc
  VoutVdc=(float)analogRead(PIN_HIH4030) / 1023.0;
  humidity=HIH4030::calculate(VoutVdc, TEMPERATURE);
  Serial.print(humidity);
  Serial.println("%");

  Serial.println("");
  delay(1000); 
}
