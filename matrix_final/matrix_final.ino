#include "Wire.h"
#include <SPI.h>
#include <RTCDS1307.h>
#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include <EEPROM.h>

// Modes
#include "Mode.h"
#include "Clock.h"
#include "RGBLED.h"
#include "Plasma.h"

// Input
#include "Button.h"

// EEPROM
#include "EEPROM.h"

#define DISP_COUNT 2

// Globals
Mode **mode=NULL;
byte modeCount=0;
byte currMode=0;
Button *button;
RTCDS1307 *rtc;
Thermistor *thermistor;
SFRGBLEDMatrix *display;

// Register a new mode
void addMode(Mode *m){
  mode=(Mode **)realloc(mode, (++modeCount) * sizeof(Mode *) );
  mode[modeCount-1]=m;
}

void setup(){
  // start facilities
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();

  // start globals
  button=new Button();
  rtc=new RTCDS1307();
  thermistor=new Thermistor(PIN_THERMISTOR);
  display=new SFRGBLEDMatrix(SIDE, DISP_COUNT, PIN_MATRIX_SS);
  display->fill(BLACK);
  display->show();
  
  // register all modes
  addMode(new Clock());
  addMode(new RGBLED());
  addMode(new Plasma);
  
  // recover last mode
  currMode=EEPROM.read(EEPROM_MODE);
  
  // done!
  Serial.println("Boot complete.");
};

void loop(){
  // cycle trhough available modes
  if(currMode>=modeCount)
    currMode=0;
  EEPROM.write(EEPROM_MODE, currMode);
  mode[currMode]->start();
  currMode++;
};


































