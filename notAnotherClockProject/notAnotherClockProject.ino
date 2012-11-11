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

#define BOOT_STEPS 7

void
setup(){
  byte step=0;

  // Prematurely light up LCD  
  pinMode(PIN_LCD_BLA, OUTPUT);
  digitalWrite(PIN_LCD_BLA, HIGH);

  // Wire
  // Wire.begin(); // ioexp.begin(); already calls it

  // SPI
  SPI.begin();

  // Sparkfun RGB LED Matrix
  delay(500); // Wait for LED Matrices to boot
  ledMatrix=new SFRGBLEDMatrix(PIN_MATRIX_SS, LEDMATRIX_HORIZ, LEDMATRIX_VERT);

  // Progress bar
  //ledMatrix->box(WHITE, 0, ledMatrix->height/2-3, ledMatrix->width, ledMatrix->height+2);
  ledMatrix->box(WHITE, 0, ledMatrix->height/2-2, ledMatrix->width-1, ledMatrix->height/2+1);
  ledMatrix->show();
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

  // MCP23017 I/O Expander
  pinMode(PIN_IOEXP_RESET, OUTPUT);
  digitalWrite(PIN_IOEXP_RESET, LOW);
  delay(50);
  digitalWrite(PIN_IOEXP_RESET, HIGH);
  ioexp.begin();
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

  // LCD12864
  // reset without u8g to allow pin to ioexp
  ioexp.pinMode(ADDR_LCD_RST, OUTPUT);
  ioexp.digitalWrite(ADDR_LCD_RST, LOW);
  delay((15*16)+2);
  ioexp.digitalWrite(ADDR_LCD_RST, HIGH);
  lcd=new U8GLIB_ST7920_128X64(PIN_LCD_E, PIN_LCD_RW, PIN_LCD_RS, U8G_PIN_NONE, U8G_PIN_NONE);
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

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
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

  // Button
  button=new Button();
  attachInterrupt(BUTTON_INT, buttonInterrupt, FALLING);
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

  // HIH4030
  HIH4030::setup(PIN_HUMIDITY);
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

  // Light
  light=new Light(PIN_LIGHT);
  light->update();
  lastLightUpdate=millis();
  analogWrite(PIN_LCD_BLA, light->read(255));
  ledMatrix->box(BLUE, 1, ledMatrix->height/2-1, (int)++step*(int)(ledMatrix->width-2)/BOOT_STEPS, ledMatrix->height/2);
  ledMatrix->show();

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
    //    MODE(3,Plasma)
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

























