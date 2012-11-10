#include <SPI.h>
#include <SFRGBLEDMatrix.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
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
#include "EEPROM.h"
#include "Clock.h"
#include "Lamp.h"
#include "Light.h"
#include "Fire.h"
#include "Plasma.h"
#include "Equalizer.h"

// Sparkfun RGB LED Matrix
SFRGBLEDMatrix *ledMatrix;
#define LEDMATRIX_HORIZ 3
#define LEDMATRIX_VERT 2

// MCP23017 I/O Expander
Adafruit_MCP23017 ioexp;

// LCD12864
U8GLIB_ST7920_128X64 *lcd;

// Button
Button *button;
boolean updateButtons=false;
void buttonInterrupt(){
  updateButtons=true;
}

// Light
Light *light;
unsigned long lastLightUpdate;

//
// setup()
//

void
setup(){
  // Wire
  // Wire.begin(); // ioexp.begin(); already calls it

  // SPI
  SPI.begin();

  // Sparkfun RGB LED Matrix
  ledMatrix=new SFRGBLEDMatrix(PIN_MATRIX_SS, LEDMATRIX_HORIZ, LEDMATRIX_VERT);
  ledMatrix->print_PF(WHITE, 3, 6, 5, PSTR("Boot"));
  ledMatrix->show();

  // MCP23017 I/O Expander
  pinMode(PIN_IOEXP_RESET, OUTPUT);
  digitalWrite(PIN_IOEXP_RESET, LOW);
  delay(50);
  digitalWrite(PIN_IOEXP_RESET, HIGH);
  ioexp.begin();

  // LCD12864
  // reset without u8g to allow pin to ioexp
  ioexp.pinMode(ADDR_LCD_RST, OUTPUT);
  ioexp.digitalWrite(ADDR_LCD_RST, LOW);
  delay((15*16)+2);
  ioexp.digitalWrite(ADDR_LCD_RST, HIGH);
  lcd=new U8GLIB_ST7920_128X64(PIN_LCD_E, PIN_LCD_RW, PIN_LCD_RS, U8G_PIN_NONE, U8G_PIN_NONE);
  lcd->setColorIndex(1); 
  lcd->firstPage();
  lcd->setFont(u8g_font_6x10);
  do {
    byte x;
    byte y;
    x=lcd->getWidth()/2-lcd->getStrWidthP(U8G_PSTR("Boot"))/2;
    y=lcd->getHeight()/2+lcd->getFontAscent()/2;
    lcd->drawStrP(x, y, U8G_PSTR("Boot"));
  } 
  while( lcd->nextPage() );
  pinMode(PIN_LCD_BLA, OUTPUT);
  digitalWrite(PIN_LCD_BLA, HIGH);

  // Button
  button=new Button();
  attachInterrupt(BUTTON_INT, buttonInterrupt, FALLING);

  // HIH4030
  HIH4030::setup(PIN_HUMIDITY);

  // Light
  light=new Light(PIN_LIGHT);
  light->update();
  lastLightUpdate=millis();
  analogWrite(PIN_LCD_BLA, light->read(255));
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
    MODE(1, Fire);
    MODE(2,Equalizer);
    MODE(3,Plasma)
      //    MODE(4, Lamp);
    default:
      EEPROM.write(EEPROM_MODE, 0);
    return;
  }
  while(1){
    mode->loop();
    // Update Buttons
    if(updateButtons){
      button->update();
      updateButtons=false;
    }
    // Light update
    if(millis()-lastLightUpdate>700){
      light->update();
      lastLightUpdate=millis();
      analogWrite(PIN_LCD_BLA, light->read(254)+1);
    }
    // Change mode
    if(button->pressed(BUTTON_MODE)){
      EEPROM.write(EEPROM_MODE, EEPROM.read(EEPROM_MODE)+1);
      break;
    }
  }
  delete mode;
}




















