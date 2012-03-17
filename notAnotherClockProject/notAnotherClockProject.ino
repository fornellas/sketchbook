#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

#include <MemoryFree.h>
#include <SFRGBLEDMatrix.h>
#include <DS1307.h>
#include <Thermistor.h>

#include "Button.h"
#include "Mode.h"
#include "pins.h"
#include "EEPROM.h"
#include "Clock.h"
#include "Fire.h"
#include "Lamp.h"

#define DISP_HORIZ 3
#define DISP_VERT 2

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
  pinMode(PIN_LINEIN_L, INPUT);
  pinMode(PIN_LINEIN_R, INPUT);
  pinMode(PIN_BUZZER, OUTPUT);

  // Global stuff
  button=new Button();
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, DISP_HORIZ, DISP_VERT);
  Mux::begin();

  // recover last mode
  currMode=EEPROM.read(EEPROM_MODE);
}

//
// loop()
//

void loop(){
  Mode *mode;

  // Save current mode
  EEPROM.write(EEPROM_MODE, currMode);
  // Create Mode object
  switch(currMode){
  case 0:
    mode=(Mode *)new Clock();
    break;
  case 1:
    mode=(Mode *)new Fire();
    break;
  case 2:
      mode=(Mode *)new Lamp();
    break;
  default:
    currMode=0;
    return; 
  }
  // loop() Mode
  while(1){
    delay(1000);
    mode->loop();
    button->update();
    if(button->pressed(BUTTON_MODE))
      break;
  }
  currMode++;
  delete mode;
  Serial.print("Mem: ");
  Serial.println(freeMemory());
  // 892 com 2 modos
  // 860 com 3 modos
}















