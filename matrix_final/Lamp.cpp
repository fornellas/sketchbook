#include "Lamp.h"
#include <SFRGBLEDMatrix.h>
#include "Button.h"

extern SFRGBLEDMatrix *display;
extern Button *button;

void Lamp::validate(int oldBrightness){
  if(brightness>MAX_C)
    brightness=MAX_C;
  if(brightness<0)
    brightness=0;
  if(brightness!=oldBrightness)
    EEPROM.write(EEPROM_LAMP_BRIGHTNESS, brightness);
}


void Lamp::loop() {
  if(button->pressed(BUTTON_A)||button->state(BUTTON_A))
    validate(brightness--);
  
  if(button->pressed(BUTTON_C)||button->state(BUTTON_C))
    validate(brightness++  );
    
    Serial.println(brightness);

  display->fill(RGB(brightness,brightness,brightness));

  display->show();
}
















