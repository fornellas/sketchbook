#include <OneWire.h>
#include <Temperature.h>
#include <DS18S20.h>

#define PIN_ONEWIRE 7
byte addr[]={40, 200, 10, 228, 3, 0, 0, 62};

OneWire *ow;
DS18S20 *temperature;

void setup(){
  Serial.begin(115200);
  ow=new OneWire(PIN_ONEWIRE);
  temperature=new DS18S20(addr, ow);
}

void loop(){
  Serial.println(temperature->readC());
  delay(1000);
}
