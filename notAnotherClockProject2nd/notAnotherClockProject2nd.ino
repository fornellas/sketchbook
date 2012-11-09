#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

#include <MemoryFree.h>
#include <SFRGBLEDMatrix.h>
#include <DS1307.h>
#include <Thermistor.h>

#include "Button.h"
#include "pins.h"
#include "EEPROM.h"
#include "Clock.h"
#include "Fire.h"
#include "Lamp.h"
#include "Plasma.h"
#include "Spectrum.h"

#define DISP_HORIZ 3
#define DISP_VERT 2

void SerialPrintPGM (PGM_P s) {
  char c;
  while ((c = pgm_read_byte(s++)) != 0)
    Serial.print(c);
}

void pMem(){
  SerialPrintPGM(PSTR("Mem: "));
  Serial.println(freeMemory());
}

//
// Global stuff
//

Button *button;
SFRGBLEDMatrix *display;
byte currMode=0;

//
// setup()
//

void setup(){
  // Arduino facilities    
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();

  // Pins (TODO Move inside class / namespace)
  pinMode(PIN_LINEIN, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Global stuff
  button=new Button();
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, DISP_HORIZ, DISP_VERT);
  display->fill(RED);
  display->show();
  Mux::begin();

  // recover last mode
  currMode=EEPROM.read(EEPROM_MODE);
  
  pMem(); // 992
}

//
// loop()
//

void loop(){
  PGM_P * (*modeEnter)() = NULL;
  void (*modeLoop)() = NULL;
  void (*modeExit)() = NULL;

  // Reset display
  display->gamma(false); 
  display->fill(BLACK);
  display->show(); 

  // Save current mode
  EEPROM.write(EEPROM_MODE, currMode);
  // Assigin mode function pointers
  // Using classes would be more elegant, but requires more memory
  switch(currMode){
  case 0:
    modeEnter=Clock::enter;
    modeLoop=Clock::loop;
    modeExit=Clock::exit;
    break;
  case 1:
    modeEnter=Fire::enter;
    modeLoop=Fire::loop;
    modeExit=Fire::exit;
    break;
  case 2:
    modeEnter=Lamp::enter;
    modeLoop=Lamp::loop;
    modeExit=Lamp::exit;
    break;
  case 3:
    modeEnter=Plasma::enter;
    modeLoop=Plasma::loop;
    modeExit=Plasma::exit;
    break;
  case 4:
    modeEnter=Spectrum::enter;
    modeLoop=Spectrum::loop;
    modeExit=Spectrum::exit;
    break;
  default:
    currMode=0;
    return; 
  }
  // Execute mode
  modeEnter();
  while(1){
    modeLoop();
    button->update();
    if(button->pressed(BUTTON_MODE))
      break;
  }
  // next mode
  modeExit();
  currMode++;
}





























