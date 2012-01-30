#include "Mode.h"
#include "Clock.h"
#include "RGBLED.h"

#include "Button.h"

Mode **mode=NULL;
byte modeCount=0;
Button *button;

void addMode(Mode *m){
//  Serial.println("Adding mode");
  mode=(Mode **)realloc(mode, (++modeCount) * sizeof(Mode *) );
  mode[modeCount-1]=m;
//    Serial.println(modeCount);
}

void setup(){
  Serial.begin(115200);
//  Serial.println("setup()");
  button=new Button();
  addMode(new Clock());
  addMode(new RGBLED());
  Serial.println("Boot complete.");
};

void loop(){
  static byte currMode=0;
//  Serial.println("loop()");

  if(currMode>=modeCount)
    currMode=0;

  mode[currMode]->main();
  currMode++;

};


































