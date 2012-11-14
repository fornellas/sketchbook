#include <SPI.h>
#include <SFRGBLEDMatrix.h>
#include <Wire.h>
#include <U8glib.h>
#include <DS1307.h>
#include <HIH4030.h>
#include <OneWire.h>
#include <DS18S20.h>
#include <BMP085.h>
#include <EEPROM.h>

#include "pins.h"
#include "Button.h"
#include "Mode.h"
#include "EEPROM_addr.h"
#include "Clock.h"
#include "Lamp.h"
#include "Light.h"
#include "Fire.h"
#include "Plasma.h"
#include "Equalizer.h"
#include "ECA.h"

// Sparkfun RGB LED Matrix
SFRGBLEDMatrix *ledMatrix;
#define LEDMATRIX_HORIZ 3
#define LEDMATRIX_VERT 2

// LCD12864
U8GLIB_ST7920_128X64 *lcd;

// Button
Button *button;
void buttonInterrupt(){
  button->update();
}

// Light
Light *light;
unsigned long lastLightUpdate;

//
// setup()
//

#define BOOT_STEPS 9

void
setup(){
  byte step=0;
  byte x;
  byte y;

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

  // Button
  button=new Button();
  attachInterrupt(INT_BUTTON_MODE, buttonInterrupt, CHANGE);
  attachInterrupt(INT_BUTTON_A, buttonInterrupt, CHANGE);
  attachInterrupt(INT_BUTTON_B, buttonInterrupt, CHANGE);
  attachInterrupt(INT_BUTTON_C, buttonInterrupt, CHANGE);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // HIH4030
  HIH4030::setup(PIN_HUMIDITY);
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Light
  light=new Light(PIN_LIGHT);
  light->update();
  lastLightUpdate=millis();
  analogWrite(PIN_LCD_BLA, light->read(255));
  ledMatrix->progressBarUpdate(BLUE, ++step, BOOT_STEPS);

  // Random
  randomSeed(BMP085::readPressure()+BMP085::readTemperature());
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
    MODE(0, Clock);
    MODE(1, Equalizer);
    MODE(2, ECA);
    MODE(3, Plasma)
    MODE(4, Lamp);
    MODE(5, Fire);
  default:
    EEPROM.write(EEPROM_MODE, 0);
    return;
  }
  while(1){
    unsigned long time;
    mode->loop();
    // Light update
    if((time=millis())-lastLightUpdate>700){
      light->update();
      lastLightUpdate=time;
      analogWrite(PIN_LCD_BLA, light->read(255-20)+20);
    }
    // Change mode
    if(button->pressed(BUTTON_MODE)){
      EEPROM.write(EEPROM_MODE, EEPROM.read(EEPROM_MODE)+1);
      break;
    }
  }
  delete mode;
}
