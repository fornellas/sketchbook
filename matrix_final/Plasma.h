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
  byte currMode;
  byte *plasma;
  void fillPlasma(byte mode);
  int pSpeed;
  void pSpeedVal();
public:
  Plasma() : 
  Mode("Plasma"){
    currMode=EEPROM.read(EEPROM_PLASMA_MODE);
    if(currMode>=PLASMA_MODES)
      currMode=0;
    pSpeed=EEPROM.read(EEPROM_PLASMA_SPEED);
    pSpeedVal();
  };
  void loop();
  void enter();
  void exit();
};

#endif
