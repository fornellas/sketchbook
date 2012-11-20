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
// Util
#include "pins.h"
#include "Button.h"
#include "EEPROM_addr.h"
#include "Net.h"
#include "Logger.h"
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
#include "HTTPServer.h"

// Sparkfun RGB LED Matrix
SFRGBLEDMatrix *ledMatrix;
#define LEDMATRIX_HORIZ 3
#define LEDMATRIX_VERT 2

// LCD12864
U8GLIB_ST7920_128X64 *lcd;

// Light
Light *light;
unsigned long lastLightUpdate;
#define BIG_CLOCK_THRESHOLD 68
int lastLightReading;
#define MODE_CLOCK 0
#define MODE_BIG_CLOCK 1
#define LIGHT_UPDATE_MS 1000

// Time
time_t bootTime;

// OneWire
OneWire *ow;

// DS18S20
DS18S20 *temperatureOutside;
byte addr[]={40, 200, 10, 228, 3, 0, 0, 62};

// BMP085
BMP085 *pressure;

// Net
Net *net;

// Logger
Logger *logger;

//
// setup()
//

#define BOOT_STEPS 16

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

  // Serial
  Serial.begin(115200);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Wire
  Wire.begin();
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

  // HIH4030
  HIH4030::setup(PIN_HUMIDITY);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Light
  light=new Light(PIN_LIGHT);
  lastLightUpdate=0;
  lastLightReading=light->read(1023);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // uptime
  DS1307 time(UTC);
  bootTime=time.getLocalTime();
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // OneWire
  ow=new OneWire(PIN_ONEWIRE);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // DS18S20
  temperatureOutside=new DS18S20(addr, ow);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // BMP085
  pressure=new BMP085(BMP085_ULTRAHIGHRES);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Ethernet
  net=new Net();
  HTTPServer::begin();
  net->addProcessor(HTTPServer::loop);
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
  randomSeed(pressure->readPa()+pressure->readC());
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
  default:
    EEPROM.write(EEPROM_MODE, 0);
    return;
  }
  while(1){
    mode->loop();
    // Ethernet
    net->processAll();
    // Light update / Logger
    if(millis()-lastLightUpdate>LIGHT_UPDATE_MS){
      light->update();
      lastLightUpdate+=LIGHT_UPDATE_MS;
      analogWrite(PIN_LCD_BLA, light->read(255-20)+20);
      // logger
      logger->update();
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






