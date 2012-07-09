#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Bike.h"

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

void setup() {    
  display.begin();
  Serial.begin(115200);
}

void loop() {
  show(Bike.getSpeed(), Bike.getDistance(), 23.4, 68);
}




























