#include <SPI.h>
#include <SFRGBLEDMatrix.h>
#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <U8glib.h>
#include <HIH4030.h>
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

//
// generalInfo
//

void
generalInfo(){
  struct DS1307::Date date;
  date=DS1307::getDate();
  float temperatureOutside;
  byte addr[]={
    40, 200, 10, 228, 3, 0, 0, 62                                                          };
  temperatureOutside=DS18S20::read(PIN_TEMP_EXT, addr);
  float temperatureInside;
  temperatureInside=BMP085::readTemperature();
  byte humidity;
  humidity=HIH4030::read(PIN_HUMIDITY, temperatureInside);
  char buff[10]; // revisar tamanho
  // LCD
  lcd->firstPage();
  do {
    byte x;
    byte y;
    // HH:MM
    byte z;
    lcd->setFont(u8g_font_fub17);    
    z=0;  
    if(date.minute<10)
      z=lcd->getStrWidthP(U8G_PSTR("0"));
    x=lcd->getWidth()/2-(
    lcd->getStrWidth(itoa(date.hour, buff, 10))+
      lcd->getStrWidthP(U8G_PSTR(":"))+
      z+
      lcd->getStrWidth(itoa(date.minute, buff, 10))
      )/2;
    y=lcd->getFontAscent();
    lcd->drawStr(x, y, itoa(date.hour, buff, 10));
    x+=lcd->getStrWidth(itoa(date.hour, buff, 10));
    lcd->drawStrP(x, y, U8G_PSTR(":"));
    x+=lcd->getStrWidthP(U8G_PSTR(":"));
    if(z){
      lcd->drawStrP(x, y, U8G_PSTR("0"));
      x+=lcd->getStrWidthP(U8G_PSTR("0"));
    }
    lcd->drawStr(x, y, itoa(date.minute, buff, 10));
    // Week day
    // Day, month year
    // Temperature outsite
    x=0;
    y=lcd->getHeight();
    lcd->drawStr(x, y, dtostrf(temperatureOutside, -2, 1, buff));
    x+=lcd->getStrWidth(dtostrf(temperatureOutside, -2, 1, buff));
    buff[0]=176;
    buff[1]='C';
    buff[2]='\0';
    lcd->drawStr(x, y, buff);
    // Temperature inside
    x=0;
    y=lcd->getHeight()-lcd->getFontAscent()-1;
    lcd->drawStr(x, y, dtostrf(temperatureInside, -2, 1, buff));
    x+=lcd->getStrWidth(dtostrf(temperatureInside, -2, 1, buff));
    buff[0]=176;
    buff[1]='C';
    buff[2]='\0';
    lcd->drawStr(x, y, buff);
    // Humidity
    x=lcd->getWidth()-lcd->getStrWidth(itoa(humidity, buff, 10))-
      lcd->getStrWidthP(U8G_PSTR("%"));
    y=lcd->getHeight();
    lcd->drawStr(x, y, itoa(humidity, buff, 10));
    x+=lcd->getStrWidth(itoa(humidity, buff, 10));
    lcd->drawStrP(x, y, U8G_PSTR("%"));

  } 
  while( lcd->nextPage() );
}

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
  pinMode(PIN_LCD_BLA, OUTPUT);
  digitalWrite(PIN_LCD_BLA, HIGH);
  ioexp.pinMode(ADDR_LCD_RST, OUTPUT);
  ioexp.digitalWrite(ADDR_LCD_RST, LOW);
  delay((15*16)+2);
  ioexp.digitalWrite(ADDR_LCD_RST, HIGH);
  lcd=new U8GLIB_ST7920_128X64(PIN_LCD_E, PIN_LCD_RW, PIN_LCD_RS, U8G_PIN_NONE, U8G_PIN_NONE);
  lcd->setColorIndex(1); 
  lcd->firstPage();
  lcd->setFont(u8g_font_fub17);
  do {
    byte x;
    byte y;
    x=lcd->getWidth()/2-lcd->getStrWidthP(U8G_PSTR("Boot"))/2;
    y=lcd->getHeight()/2+lcd->getFontAscent()/2;
    lcd->drawStrP(x, y, U8G_PSTR("Boot"));
  } 
  while( lcd->nextPage() );

  // Button
  button=new Button();
  attachInterrupt(BUTTON_INT, buttonInterrupt, FALLING);

  // HIH4030
  HIH4030::setup(PIN_HUMIDITY);
}

//
// loop()
//

void
loop(){
  Mode *mode;

  // Modes
  switch(EEPROM.read(EEPROM_MODE)){
  case 0:
    mode=new Clock();
    break;
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
    // LCD FIXME
    generalInfo();
    // TODO detect night mode
    // Change mode
    if(button->pressed(BUTTON_MODE)){
      EEPROM.write(EEPROM_MODE, EEPROM.read(EEPROM_MODE)+1);
      break;
    }
  }
  delete mode;
}









