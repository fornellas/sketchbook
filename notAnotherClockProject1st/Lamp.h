#ifndef Lamp_h
#define Lamp_h

#include "Mode.h"
#include <EEPROM.h>
#include "EEPROM.h"

class Lamp: 
public Mode {
private:
  int brightness;
  void validate(int);
public:
  Lamp() : 
  Mode("Lamp"){
    brightness=EEPROM.read(EEPROM_LAMP_BRIGHTNESS);
    validate(-1);
  };
  void loop();
};

#endif

