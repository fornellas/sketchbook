#ifndef Mode_h
#define Mode_h

#include <Arduino.h>

#define LOOP_HZ 60

class Mode {
public:
  Mode(String name);
  void main();
  String name;
  virtual void enter(){};
  virtual void loop(){};
  virtual void exit(){};
};

#endif

