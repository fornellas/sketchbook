#ifndef Plasma_h
#define Plasma_h

#include "Mode.h"
#include <Arduino.h>
#include <SFRGBLEDMatrix.h>

#define PLASMA_MODES 4

class Plasma: 
public Mode {
private:
  Color spectrum(byte p);
  byte *plasma;
  double cubicInterpolate (double *p, double x);
  double bicubicInterpolate(double *pixmap, uint8_t pixmapWidth, uint8_t pixmapHeight, uint8_t pointX, uint8_t pointY, double x, double y);
  void fillPlasma();
  int pSpeed;
  void pSpeedValidate();
public:
  Plasma();
  void loop();
  ~Plasma();
};

#endif













