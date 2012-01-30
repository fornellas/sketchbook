#ifndef Mode_h
#define Mode_h

#include <Arduino.h>

#define LOOP_HZ 60

// Parent class for all modes

class Mode {
private:
  // mode name
  String name;
public:
  // Must receive mode
  Mode(String name);
  // This method starts running the mode
  void start();

  // These methods are to be implemented for each mode and are called inside start()

  // Called when entering the mode
  virtual void enter(){
  };
  // Thil will be called in a loop
  virtual void loop(){
  };
  // Called when exiting the mode
  virtual void exit();
};

#endif



