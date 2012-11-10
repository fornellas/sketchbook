#ifndef Mode_h
#define Mode_h

#include <avr/pgmspace.h>

class Mode {
public:
  const char *name;
  Mode(const char *name);
  virtual void loop()=0;
  virtual ~Mode();
};

#endif



