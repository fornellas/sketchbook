#include <HIH4030.h>

void HIH4030::setup(uint8_t pin){
  pinMode(pin, INPUT);
  analogReference(DEFAULT);
}

float HIH4030::read(uint8_t pin, float temperature){
  return calculate(float(analogRead(pin))/1023.0, temperature);
}

float HIH4030::calculate(float VoutVdc, float temperature){
  return (VoutVdc-0.16)/(0.00653852-0.000013392*temperature);
}


