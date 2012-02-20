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
    // call main mode method
    this->loop();

    // read buttons
    button->update();

    // change mode if MODE button is pressed
    if(button->pressed(MODE)) {
      this->exit();
      return;
    }
  }
}

void Mode::exit() {
  display->fill(BLACK);
  display->show(); 
}






