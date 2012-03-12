#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <EEPROM.h>

#include <MemoryFree.h>
#include <SFRGBLEDMatrix.h>

#include "Button.h"
#include "Mode.h"
#include "pins.h"
#include "EEPROM.h"
#include "Clock.h"

//
// Global stuff
//

Button *button;
SFRGBLEDMatrix *display;

//
// setup()
//

void setup(){
  // Arduino facilities                                                                      
  Serial.begin(115200);
  Wire.begin();
  SPI.begin();

  // Global stuff
  button=new Button();
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, 3, 2);
  
}

//
// loop()
//

void loop(){
  Serial.println(freeMemory());
  nacp->loop();
}




