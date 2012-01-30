#include "Mode.h"
#include "Button.h"

extern Button *button;

Mode::Mode(String name) {
  this->name=name;
}

void Mode::main() {
  boolean change=false;

  Serial.print("Mode changed: ");
  Serial.print(name);
  Serial.println("");

  this->enter();

  while(1){
    //    Serial.println("Reading buttons.");
    button->update();
    if(button->state(MODE)==false)
      change=true;
    if(change&&button->state(MODE)) {
      this->exit();
      return;
    }
    this->loop();
    delay(1000/LOOP_HZ);
  }
}






