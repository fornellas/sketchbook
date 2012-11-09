#include <Wire.h>
#include <Adafruit_MCP23017.h>
#include <SPI.h>
#include <SFRGBLEDMatrix.h>

#include "pins.h"
#include "Button.h"

// MCP23017 I/O Expander
Adafruit_MCP23017 ioexp;

// Sparkfun RGB LED Matrix
SFRGBLEDMatrix *display;
#define DISP_HORIZ 3
#define DISP_VERT 2

// Button
Button *button;
boolean updateButtons=false;
void buttonInterrupt(){
  updateButtons=true;
}



void
setup(){
  // Wire
  // Wire.begin(); // ioexp.begin(); already calls it

  // SPI
  SPI.begin();

  // MCP23017 I/O Expander
  pinMode(PIN_IOEXP_RESET, OUTPUT);
  digitalWrite(PIN_IOEXP_RESET, LOW);
  delay(50);
  digitalWrite(PIN_IOEXP_RESET, HIGH);
  ioexp.begin();

  // Sparkfun RGB LED Matrix
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, DISP_HORIZ, DISP_VERT);
  display->print_PF(WHITE, 3, 6, 5, PSTR("BOOT"));
  display->show();

  // Button
  button=new Button();
  attachInterrupt(BUTTON_INT, buttonInterrupt, CHANGE);
}
byte c;
void
loop(){
  display->fill(BLACK);

  if(updateButtons){
    display->paintPixel(ORANGE,0,1);
    button->update();
    updateButtons=false;
  }

  if(button->state(BUTTON_MODE))
    display->paintPixel(RED,0,0);
  if(button->state(BUTTON_A))
    display->paintPixel(GREEN,1,0);
  if(button->state(BUTTON_B))
    display->paintPixel(BLUE,2,0);
  if(button->state(BUTTON_C))
    display->paintPixel(WHITE,3,0);
    
    display->print(WHITE,0,8,5,c++);
  display->show(); 
}















