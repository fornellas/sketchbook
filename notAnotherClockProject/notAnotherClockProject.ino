// AVR Libc
#include <avr/interrupt.h>
// Arduino
#include <SPI.h>
#include <Wire.h>
#include <EEPROM.h>
#include <SD.h>
// Custom
#include <SFRGBLEDMatrix.h>
#include <U8glib.h>
#include <Time.h>
#include <DS1307.h>
#include <Humidity.h>
#include <HIH4030.h>
#include <OneWire.h>
#include <Temperature.h>
#include <DS18S20.h>
#include <Pressure.h>
#include <BMP085.h>
#include <Ethernet.h>
#include <EthernetInterrupt.h>
#include <MemoryFree.h>
#include <WebServer.h>
#include <DHT22.h>
// Util
#include "pins.h"
#include "Button.h"
#include "EEPROM_addr.h"
// Modes
#include "Mode.h"
#include "Clock.h"
#include "Lamp.h"
#include "Light.h"
#include "Fire.h"
#include "Plasma.h"
#include "Equalizer.h"
#include "ECA.h"
#include "BigClock.h"
#include "Spore.h"
// Net
#include "Net.h"
#include "HTTPServer.h"
#include "WUnder.h"
// Misc
#include "Logger.h"

// Sparkfun RGB LED Matrix
SFRGBLEDMatrix *ledMatrix;
#define LEDMATRIX_HORIZ 3
#define LEDMATRIX_VERT 2

// Time
time_t bootTime;

// LCD12864
U8GLIB_ST7920_128X64 *lcd;

// Light
Light *light;
unsigned long lastLightUpdate;
#define BIG_CLOCK_THRESHOLD 50
int lastLightReading;
#define MODE_CLOCK 0
#define MODE_BIG_CLOCK 1
#define LIGHT_UPDATE_MS 1000

// OneWire
OneWire *ow;

// DS18S20
byte DS18S20_addr[]={40, 200, 10, 228, 3, 0, 0, 62};
DS18S20 *ds18s20;
Temperature *temperatureOutside2;

// BMP085
BMP085 *bmp085;
Pressure *pressure;
Temperature *temperatureInside;

// HIH4030
HIH4030 *hih4030;
Humidity *humidityInside;

// DHT22
DHT22 *dht22;
Temperature *temperatureOutside;
Humidity *humidityOutside;

// Net
Net *net;
HTTPServer *server;
WUnder *wunder;

// Logger
Logger *logger;

//
// setup()
//

#define BOOT_STEPS 18

void
setup(){
  byte step=0;

  // Wait for LED Matrices to boot
  delay(800);

  // SPI
  SPI.begin();

  // Sparkfun RGB LED Matrix
  ledMatrix=new SFRGBLEDMatrix(PIN_MATRIX_SS, LEDMATRIX_HORIZ, LEDMATRIX_VERT);

  // Progress bar
  ledMatrix->progressBarInit(WHITE);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Wire
  Wire.begin();
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // uptime
  DS1307 time(UTC);
  bootTime=time.getLocalTime();
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Serial
  Serial.begin(115200);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // LCD12864
  // reset without u8g to allow pin to ioexp
  lcd=new U8GLIB_ST7920_128X64(PIN_LCD_E, PIN_LCD_RW, PIN_LCD_RS, U8G_PIN_NONE, PIN_LCD_RST);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  lcd->setColorIndex(1); 
  lcd->firstPage();
  do {
  } 
  while( lcd->nextPage() );
  // light up LCD  
  pinMode(PIN_LCD_BLA, OUTPUT);
  digitalWrite(PIN_LCD_BLA, HIGH);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Light
  light=new Light(PIN_LIGHT);
  lastLightUpdate=0;
  lastLightReading=light->read(1023);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // OneWire
  ow=new OneWire(PIN_ONEWIRE);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // DS18S20
  ds18s20=new DS18S20(DS18S20_addr, ow);
  temperatureOutside2=ds18s20;
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // BMP085
  bmp085=new BMP085(BMP085_ULTRAHIGHRES);
  temperatureInside=bmp085;
  pressure=bmp085;
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // HIH4030
  hih4030=new HIH4030(PIN_HUMIDITY, bmp085);
  humidityInside=hih4030;
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // DHT22
  dht22=new DHT22(PIN_DHT22);
  temperatureOutside=dht22;
  humidityOutside=dht22;
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Net
  net=new Net();
  server=new HTTPServer();
  wunder=new WUnder();
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Logger
  logger=new Logger();
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // SD
  if(SD.begin(PIN_CS_USD))
    ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);
  else{
    ledMatrix->progressBarUpdate(RED, ++step, BOOT_STEPS); // FIXME better error reporting
    while(1);
  }

  // Random
  bmp085->loadFromSensor();
  randomSeed(pressure->getPa()+temperatureInside->getC());
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // RGB LED
  pinMode(PIN_R, OUTPUT);
  pinMode(PIN_G, OUTPUT);
  pinMode(PIN_B, OUTPUT);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);
}

//
// loop()
//

void
loop(){
  Mode *mode;
  // Modes
#define MODE(number, name) case number:mode=new name();EEPROM.write(EEPROM_MODE, number);break;
  switch(EEPROM.read(EEPROM_MODE)){
    MODE(MODE_CLOCK, Clock);
    MODE(MODE_BIG_CLOCK, BigClock);
    MODE(2, ECA);
    MODE(3, Plasma);
    MODE(4, Lamp);
    MODE(5, Fire);
    MODE(6, Equalizer);
    MODE(7, Spore);
  default:
    EEPROM.write(EEPROM_MODE, 0);
    return;
  }
  while(1){
    mode->loop();
    // Ethernet
    net->processAll();
    // Light update / Logger / WUnder
    if(millis()-lastLightUpdate>LIGHT_UPDATE_MS){
      // logger
      logger->update();
      // WUnder
      wunder->update();
      // Light
      light->update();
      lastLightUpdate+=LIGHT_UPDATE_MS;
      analogWrite(PIN_LCD_BLA, light->read(255-20)+20);
      // Change to Big Clock when it gets dark
      if(lastLightReading>BIG_CLOCK_THRESHOLD && light->read(1023)<=BIG_CLOCK_THRESHOLD) {
        lastLightReading=light->read(1023);
        EEPROM.write(EEPROM_MODE, MODE_BIG_CLOCK);
        break;
      }
      // Return to Clock when it is light again
      if(lastLightReading<=BIG_CLOCK_THRESHOLD && light->read(1023)>=BIG_CLOCK_THRESHOLD){
        lastLightReading=light->read(1023);
        EEPROM.write(EEPROM_MODE, MODE_CLOCK);
        break;
      }
    }
    // Change mode
    if(button.pressed(BUTTON_MODE)){
      EEPROM.write(EEPROM_MODE, EEPROM.read(EEPROM_MODE)+1);
      break;
    }
  }
  delete mode;
}
