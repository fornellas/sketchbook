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
#include <DS1307.h>
#include <HIH4030.h>
#include <OneWire.h>
#include <DS18S20.h>
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

// Net
Net *net;

//
// setup()
//

#define BOOT_STEPS 11

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

  // Ethernet
  net=new Net();
  HTTPServer::begin();
  net->addProcessor(HTTPServer::loop);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // SD
  if(SD.begin(PIN_CS_USD))
    ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);
  else{
    ledMatrix->progressBarUpdate(RED, ++step, BOOT_STEPS);
    while(1);
  }

  // Random
  randomSeed(BMP085::readPressure()+BMP085::readTemperature());
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
    unsigned long time;
    mode->loop();
    // Ethernet
    net->processAll();
    // Light update
    if((time=millis())-lastLightUpdate>700){
      light->update();
      lastLightUpdate=time;
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






