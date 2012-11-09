#include <DHT.h>
#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>
#include "Bike.h"

#define DHTPIN 4     // what pin we're connected to

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11 
#define DHTTYPE DHT22   // DHT 22  (AM2302)
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

DHT dht(DHTPIN, DHTTYPE);

// VCC
// GND
// pin 4 - LCD chip select (SCE)
// pin 3 - LCD reset (RST)
// pin 5 - Data/Command select (D/C)
// pin 6 - Serial data out (MOSI)
// pin 7 - Serial clock out (SCLK)
// LED
Adafruit_PCD8544 display = Adafruit_PCD8544(13, 12, 7, 8, 10);

void setup() {    
  display.begin();
  Serial.begin(115200);
  pinMode(6, OUTPUT);
  digitalWrite(6, HIGH);
}

void loop() {
  show(Bike.getSpeed(), Bike.getDistance(), dht.readTemperature(), dht.readHumidity());
}




























