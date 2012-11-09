#ifndef Button_h
#define Button_h

#include "pins.h"
#include "Mux.h"

#define BUTTON_COUNT 4

#define BUTTON_MODE 0
#define BUTTON_A 1
#define BUTTON_B 2
#define BUTTON_C 3

class Button {
private:
  boolean buttonState[BUTTON_COUNT];
  unsigned long lastChangeTime[BUTTON_COUNT];
  boolean pressedState[BUTTON_COUNT];
  boolean releasedState[BUTTON_COUNT];
public:
  Button();
  // to be called in loop to update button status
  void update();
  // get the status of a button
  boolean state(byte button);
  // true if button has been pressed
  boolean pressed(byte button);
  boolean released(byte button);
};

#endif
