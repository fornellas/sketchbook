#include "Wire.h"
#include <SPI.h>
#include <RTCDS1307.h>
#include <SFRGBLEDMatrix.h>
#include <Thermistor.h>
#include <EEPROM.h>

// Modes
#include "Mode.h"
#include "Clock.h"
#include "Plasma.h"

// Input
#include "Button.h"

// EEPROM
#include "EEPROM.h"

#define DISP_COUNT 5

// Globals
Mode **mode=NULL;
byte modeCount=0;
byte currMode=0;
Button *button;
RTCDS1307 *rtc;
SFRGBLEDMatrix *display;

// Register a new mode
void addMode(Mode *m){
  mode=(Mode **)realloc(mode, (++modeCount) * sizeof(Mode *) );
  mode[modeCount-1]=m;
}

void setup(){
  //
  // start globals
  //

  // Arduino facilities
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();

  // CD74HC4067
  pinMode(PIN_CD74HC4067_S0, OUTPUT);
  pinMode(PIN_CD74HC4067_S1, OUTPUT);
  pinMode(PIN_CD74HC4067_S2, OUTPUT);
  pinMode(PIN_CD74HC4067_S3, OUTPUT); 

  // Common classes
  button=new Button();
  rtc=new RTCDS1307();
  //  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, DISP_COUNT);
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, 3, 2);
  display->show();

  //
  // Modes
  //

  // register modes
  addMode(new Clock());
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



































