#ifndef Plasma_h
#define Plasma_h

#include "Mode.h"
#include <EEPROM.h>
#include "EEPROM.h"
#include <SFRGBLEDMatrix.h>

#define PLASMA_MODES 4

extern SFRGBLEDMatrix *display;

class Plasma: 
public Mode {
private:
  byte *plasma;
  void fillPlasma();
  int pSpeed;
  void pSpeedValidate();
public:
  Plasma() : 
  Mode("Plasma"){
    pSpeed=EEPROM.read(EEPROM_PLASMA_SPEED);
    pSpeedValidate();
  };
  void loop();
  void enter();
  void exit();
};

#endif
