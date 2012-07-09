#include "Bike.h"

#define WHEEL_DIAMETER 2.09 // in meters
#define INT_WHEEL 0 // PIN D2
#define MAX_SPEED 80.0 // in km/h, for hall effect sensor noise filtering
#define MIN_REV_US 1E6/((MAX_SPEED/3.6)/WHEEL_DIAMETER) // minimum revolution time
#define SPEED_UPDATE_MS 500

BikeClass Bike;

BikeClass::BikeClass(){
  intMicros=0;
  lock=false;
  lastRevolutionsIncrementMicros=0;
  currRevolutions=0;
  lastSpeedCalcMicros=0;
  lastSpeedCalcRevolutions=0;
  lastSpeedUpdateMicros=0;
  lastSpeedNotCalcMicros=0;
  resetRevolutions=0;
  attachInterrupt(INT_WHEEL, incrementRevolutions, FALLING);
}

void BikeClass::updateSpeed(unsigned long currMicros, unsigned long revolutions){
  if(currMicros-lastRevolutionsIncrementMicros>MIN_REV_US){
    currRevolutions+=revolutions;
    lastRevolutionsIncrementMicros=currMicros;
    if(currMicros-lastSpeedCalcMicros>SPEED_UPDATE_MS*1E3){
      currSpeed=(WHEEL_DIAMETER*(currRevolutions-lastSpeedCalcRevolutions))/(float((currMicros-lastSpeedCalcMicros))/1E6)*3.6;
      lastSpeedCalcMicros=currMicros;
      lastSpeedUpdateMicros=currMicros;
      lastSpeedCalcRevolutions=currRevolutions;
    }
  }
  else
    lastSpeedNotCalcMicros=currMicros;
}

void BikeClass::incrementRevolutions(){
  unsigned long us;
  us=micros();
  if(Bike.lock)
    Bike.intMicros=us;
  else
    Bike.updateSpeed(us, 1);
}

float BikeClass::getSpeed(){
  unsigned long currMicros;
  float estimatedSpeed;

  lock=true;
  currMicros=micros();
  if(currMicros-lastSpeedUpdateMicros>SPEED_UPDATE_MS*1E3){
    if(currRevolutions==lastSpeedCalcRevolutions){
      estimatedSpeed=WHEEL_DIAMETER/(float((currMicros-lastSpeedCalcMicros))/1E6)*3.6;
    }
    else
      estimatedSpeed=(WHEEL_DIAMETER*(currRevolutions-lastSpeedCalcRevolutions))/(float((currMicros-lastSpeedNotCalcMicros))/1E6)*3.6;
    if(estimatedSpeed<currSpeed){
      lastSpeedUpdateMicros=currMicros;
      currSpeed=estimatedSpeed;
    }
  }
  if(intMicros){
    updateSpeed(intMicros, 1);
    intMicros=0; 
  }
  lock=false;

  return currSpeed;
}

float BikeClass::getDistance(){
  return WHEEL_DIAMETER*float(currRevolutions-resetRevolutions)/1000.0;
}

void BikeClass::resetDistance(){
  resetRevolutions=currRevolutions;
}
