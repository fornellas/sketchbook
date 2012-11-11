#ifndef Mode_h
#define Mode_h

#include <avr/pgmspace.h>
#include <Arduino.h>

class Mode {
  private:
  boolean static first;
public:
  PROGMEM char *name;
  Mode(PROGMEM char *name);
  virtual void loop()=0;
  virtual ~Mode();
};

#endif



