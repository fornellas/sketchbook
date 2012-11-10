#include "Mode.h"

#include <SFRGBLEDMatrix.h>

extern SFRGBLEDMatrix *ledMatrix;

Mode::Mode(const char *name) {
  this->name=name;
  // RESET LED Matrix
  ledMatrix->gamma(false); 
  ledMatrix->fill(BLACK);
  ledMatrix->show(); 
  //TODO entrance animation
}

Mode::~Mode() {
  // TODO ending animation
}


