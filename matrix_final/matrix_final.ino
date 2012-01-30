// Modes
#include "Mode.h"
#include "Clock.h"
#include "RGBLED.h"

// Input
#include "Button.h"

// Globals
Mode **mode=NULL;
byte modeCount=0;
Button *button;

// Register a new mode
void addMode(Mode *m){
  mode=(Mode **)realloc(mode, (++modeCount) * sizeof(Mode *) );
  mode[modeCount-1]=m;
}

void setup(){
  // start serial
  Serial.begin(115200);

  // start input
  button=new Button();
  
  // register all modes
  addMode(new Clock());
  addMode(new RGBLED());
  
  // done!
  Serial.println("Boot complete.");
};

void loop(){
  static byte currMode=0;
  
  // cycle trhough available modes
  if(currMode>=modeCount)
    currMode=0;
  mode[currMode]->start();
  currMode++;
};


































