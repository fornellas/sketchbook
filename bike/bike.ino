#include <Adafruit_GFX.h>
#include <Adafruit_PCD8544.h>

#define WHEEL_DIAMETER 2.09
#define INT_WHEEL 0 // PIN D2
#define MAX_SPEED 80.0
#define MIN_REV_US 1E6/((MAX_SPEED/3.6)/WHEEL_DIAMETER)
#define SPEED_UPDATE_MS 500

// pin 7 - Serial clock out (SCLK)
// pin 6 - Serial data out (DIN)
// pin 5 - Data/Command select (D/C)
// pin 4 - LCD chip select (CS)
// pin 3 - LCD reset (RST)
Adafruit_PCD8544 display = Adafruit_PCD8544(7, 6, 5, 4, 3);

volatile unsigned long currRevolutions=0;
volatile unsigned long lastRevolutionsIncrementMicros=0;
volatile float currSpeed;
volatile unsigned long lastSpeedUpdateMicros=0;
volatile unsigned long lastSpeedUpdateRevolutions=0;
unsigned long lastEstimatedSpeedMicros=0;
volatile boolean isrLock=false;

void incrementRevolutions(){
  unsigned long currMicros;
  
  currMicros=micros();
  if(currMicros-lastRevolutionsIncrementMicros>MIN_REV_US){
    currRevolutions++;
    lastRevolutionsIncrementMicros=currMicros;
    if(currMicros-lastSpeedUpdateMicros>SPEED_UPDATE_MS*1E3){
      currSpeed=(WHEEL_DIAMETER*(currRevolutions-lastSpeedUpdateRevolutions))/(float((currMicros-lastSpeedUpdateMicros))/1E6)*3.6;
      lastSpeedUpdateMicros=currMicros;
      lastSpeedUpdateRevolutions=currRevolutions;
    }
  }
}

void show(float speed, float distance, float temperature, float humidity){
  byte d;
  display.clearDisplay();

  display.drawRect(70, 0, 84-70, 6, BLACK);
  display.drawFastVLine(69, 2, 2, BLACK);
  display.drawFastVLine(68, 2, 2, BLACK);
  display.drawRect(72, 2, 84-70-4, 2, BLACK);

  display.setTextSize(3);
  if(speed<10.0)
    display.setCursor(18,0);
  else
    display.setCursor(3,0);
  display.println(int(speed));
  display.setTextSize(2);
  display.setCursor(35,7);
  display.println(".");
  display.setCursor(45,7);
  d=floor(10.0*(speed-floor(speed)));
  display.println(d);
  display.setTextSize(1);
  display.setCursor(57,14);
  display.println("Km/h");

  display.drawFastHLine(0, 22, 84, BLACK);

  display.setTextSize(2);
  if(distance<10){
    display.setCursor(12,24);
    display.println(distance, 2);
    display.setTextSize(1);
    display.setCursor(61, 31);
  }
  else if(distance<100){
    display.setCursor(6,24);
    display.println(distance, 2);
    display.setTextSize(1);
    display.setCursor(67, 31);
  }
  else{
    display.setCursor(7,24);
    display.println(distance, 1);
    display.setTextSize(1);
    display.setCursor(67, 31);
  }
  display.println("km");

  display.drawFastHLine(0, 39, 84, BLACK);

  display.setTextSize(1);
  display.setCursor(4,41);
  display.println(temperature, 1);
  display.setCursor(26,36);
  display.println(".");
  display.setCursor(31,41);
  display.println("C");

  display.drawFastVLine(42, 40, 8, BLACK);

  display.setCursor(54,41);
  display.println(humidity, 0);
  display.setCursor(66,41);
  display.println("%");

  display.display();
}

void setup() {    
  display.begin();
  attachInterrupt(INT_WHEEL, incrementRevolutions, FALLING);
  Serial.begin(115200);
}

void loop() {
  float estimatedSpeed;
  float speed;
  unsigned long currMicros;
  currMicros=micros();

  speed=currSpeed; 
  Serial.println("");
  Serial.print("currSpeed: ");
  Serial.println(speed);
  
  // Race condition nas variaveis do ISLR
  if(currMicros-lastSpeedUpdateMicros>SPEED_UPDATE_MS*1E3 && currMicros-lastEstimatedSpeedMicros>SPEED_UPDATE_MS*1E3){
    Serial.println("  currMicros-lastSpeedUpdateMicros>SPEED_UPDATE_MS*1E3&&currMicros-lastEstimatedSpeedMicros>SPEED_UPDATE_MS*1E3");
    unsigned long r;
    Serial.print("  currRevolutions = ");
    Serial.println(currRevolutions);
    Serial.print("  lastSpeedUpdateRevolutions = ");
    Serial.println(lastSpeedUpdateRevolutions);
    Serial.print("  currMicros = ");
    Serial.println(currMicros); 
    Serial.print("  lastSpeedUpdateMicros = ");
    Serial.println(lastSpeedUpdateMicros);

    r=currRevolutions-lastSpeedUpdateRevolutions;
    if(r==0)
      r=1;
    estimatedSpeed=(WHEEL_DIAMETER*r)/(float((currMicros-lastSpeedUpdateMicros))/1E6)*3.6;
    if(estimatedSpeed<currSpeed){
      lastEstimatedSpeedMicros=currMicros;
      speed=estimatedSpeed;
      currSpeed=estimatedSpeed;
      Serial.print("    ESTIMATED: ");
      Serial.println(speed);
    }
  }

  // fix for -0.00
  if(speed<0)
    speed=0;

  show(speed, WHEEL_DIAMETER*float(currRevolutions)/1000.0, 23.4, 68);

}



