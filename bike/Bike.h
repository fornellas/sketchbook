#ifndef Bike_h
#define Bike_h

#include <Arduino.h>

class BikeClass {
private:
  volatile boolean lock;
  volatile unsigned long intMicros;
  volatile unsigned long lastRevolutionsIncrementMicros;
  volatile unsigned long currRevolutions;
  volatile unsigned long lastSpeedCalcMicros;
  volatile float currSpeed;
  volatile unsigned long lastSpeedCalcRevolutions;
  volatile unsigned long lastSpeedUpdateMicros;
  volatile unsigned long lastSpeedNotCalcMicros;
  unsigned long resetRevolutions;
  void updateSpeed(unsigned long currMicros, unsigned long revolutions);
  static void incrementRevolutions();
public:
  BikeClass();
  float getSpeed();
  float getDistance();
  void resetDistance();
};

extern BikeClass Bike;

#endif


