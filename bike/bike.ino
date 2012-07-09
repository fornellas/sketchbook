#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define WHEEL_DIAMETER 2.09 // in meters
#define INT_WHEEL 0 // PIN D2
#define MAX_SPEED 80.0 // in km/h, for hall effect sensor noise filtering
#define MIN_REV_US 1E6/((MAX_SPEED/3.6)/WHEEL_DIAMETER) // minimum revolution time
#define SPEED_UPDATE_MS 500

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

volatile unsigned long intMicros=0;

volatile boolean lock=false;

volatile unsigned long lastRevolutionsIncrementMicros=0;
volatile unsigned long currRevolutions=0;
volatile unsigned long lastSpeedCalcMicros=0;
volatile float currSpeed;
volatile unsigned long lastSpeedCalcRevolutions=0;
volatile unsigned long lastSpeedUpdateMicros=0;
volatile unsigned long lastSpeedNotCalcMicros=0;

void updateSpeed(unsigned long currMicros, unsigned long revolutions){
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

void incrementRevolutions(){
  unsigned long us;
  us=micros();
  if(lock)
    intMicros=us;
  else
    updateSpeed(us, 1);
}

void setup() {    
  display.begin();
  attachInterrupt(INT_WHEEL, incrementRevolutions, FALLING);
  Serial.begin(115200);
}

void loop() {
  float distance;
  unsigned long currMicros;
  float estimatedSpeed;

  lock=true;
  currMicros=micros();
  Serial.print(currMicros);
  Serial.print("\t");
  Serial.print(currRevolutions);
  Serial.print("\t");
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

  distance=WHEEL_DIAMETER*float(currRevolutions)/1000.0;
  show(currSpeed, distance, 23.4, 68);

  Serial.println(currSpeed);
}




























