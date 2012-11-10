#include "Mode.h"

#include <SFRGBLEDMatrix.h>

extern SFRGBLEDMatrix *ledMatrix;

Mode::Mode(const char *name) {
  this->name=name;
  //TODO entrance animation
}

Mode::~Mode() {
  // TODO ending animation
}

