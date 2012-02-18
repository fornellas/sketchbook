#include "Mode.h"
#include "Button.h"
#include <SFRGBLEDMatrix.h>
extern SFRGBLEDMatrix *display;

extern Button *button;

Mode::Mode(String name) {
  this->name=name;
}

void Mode::start() {
  Serial.print("Mode changed: ");
  Serial.print(name);
  Serial.println("");

  this->enter();

  while(1){
    // read buttons
    button->update();

    // change mode if MODE button is pressed
    if(button->released(MODE)) {
      this->exit();
      return;
    }

    // call main mode method
    this->loop();

    // sleep
    delay(1000/LOOP_HZ);
  }
}

void Mode::exit() {
  display->fill(BLACK);
  display->show(); 
}






