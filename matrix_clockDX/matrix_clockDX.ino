#include <Thermistor.h>
#include <SFRGBLEDMatrix.h>
#include <RTCDS1307.h>
#include "Wire.h"
#include <SPI.h>

#define PIN_MATRIX_SS   10

#define DISP_COUNT 2

#define APIN_PHOTORESISTOR 1
#define APIN_THERMISTOR 0

#define PAINT(x, y, c) display->frameBuff[display->width*y+x]=c

void printHour(SFRGBLEDMatrix *display, int rg_brightness, int hour){
  byte r=RGB(rg_brightness,0,0);
  if(hour>=1||hour==0)
    PAINT(5,0,r);
  if(hour>=2||hour==0)
    PAINT(6,1,r);
  if(hour>=3||hour==0)
    PAINT(6,3,r);
  if(hour>=4||hour==0)
    PAINT(6,5,r);
  if(hour>=5||hour==0)
    PAINT(5,6,r);
  if(hour>=6||hour==0)
    PAINT(3,6,r);
  if(hour>=7||hour==0)
    PAINT(1,6,r);
  if(hour>=8||hour==0)
    PAINT(0,5,r);
  if(hour>=9||hour==0)
    PAINT(0,3,r);
  if(hour>=10||hour==0)
    PAINT(0,1,r);
  if(hour>=11||hour==0)
    PAINT(1,0,r);
  if(hour>=12||hour==0)
    PAINT(3,0,r);
};

void printMinute(SFRGBLEDMatrix *display, int rg_brightness, int minute){
  byte y=RGB(rg_brightness,rg_brightness,0);
  byte g=RGB(0,rg_brightness,0);
  if(minute>=5)
    PAINT(4,1,y);
  if(minute>=10)
    PAINT(5,2,y);
  if(minute>=15)
    PAINT(5,3,y);
  if(minute>=20)
    PAINT(5,4,y);
  if(minute>=25)
    PAINT(4,5,y);
  if(minute>=30)
    PAINT(3,5,y);
  if(minute>=35)
    PAINT(2,5,y);
  if(minute>=40)
    PAINT(1,4,y);
  if(minute>=45)
    PAINT(1,3,y);
  if(minute>=50)
    PAINT(1,2,y);
  if(minute>=55)
    PAINT(2,1,y);
  if(minute>=60)
    PAINT(3,1,y);

  if(minute%5>=1)
    PAINT(3,2,g);
  if(minute%5>=2)
    PAINT(4,3,g);
  if(minute%5>=3)
    PAINT(3,4,g);
  if(minute%5>=4)
    PAINT(2,3,g);
};

void printSec(SFRGBLEDMatrix *display, int rg_brightness, int b_brightness, int second){
  byte c=RGB(0,rg_brightness,b_brightness);
  byte b=RGB(0,0,b_brightness);
  /*
  if(second>=5)
   PAINT(4,0,c);
   if(second>=10)
   PAINT(5,1,c);
   if(second>=15)
   PAINT(6,2,c);
   if(second>=20)
   PAINT(6,4,c);
   if(second>=25)
   PAINT(5,5,c);
   if(second>=30)
   PAINT(4,6,c);
   if(second>=35)
   PAINT(2,6,c);
   if(second>=40)
   PAINT(1,5,c);
   if(second>=45)
   PAINT(0,4,c);
   if(second>=50)
   PAINT(0,2,c);
   if(second>=55)
   PAINT(1,1,c);
   if(second>=60)
   PAINT(2,0,c);
   */
  if(second%5>=0)
    PAINT(3,3,b);
  if(second%5>=1)
    PAINT(4,2,b);
  if(second%5>=2)
    PAINT(4,4,b);
  if(second%5>=3)
    PAINT(2,4,b);
  if(second%5>=4)
    PAINT(2,2,b);
};

void printTime(SFRGBLEDMatrix *display, int rg_brightness, int hour, int minute, int second){
  int p;
  byte b_brightness=3;
  if(rg_brightness<3)
    b_brightness=rg_brightness/2;
  if(!b_brightness)
    b_brightness=1;
  for(p=0;p<display->pixels;p++)
    display->frameBuff[p]=BLACK;
  //  PAINT(3,3,RGB(0,rg_brightness,0));
  if(hour<13)
    printHour(display, rg_brightness, hour);
  else
    printHour(display, rg_brightness, hour-12);
  printMinute(display, rg_brightness, minute);
  printSec(display, rg_brightness, b_brightness, second);
};


void setup(){
  SPI.begin();
  Wire.begin();
}

void loop(){
  int p;
  struct Date d;
  int k=0;

  SFRGBLEDMatrix display(SIDE, DISP_COUNT, PIN_MATRIX_SS);
  RTCDS1307 rtc = new RTCDS1307();
  display.config();
  int j=0;
  float temperature;
  Thermistor thermistor(APIN_THERMISTOR);
#define temperatureBarSize 29
  byte temperatureBar[]={
    RGB(7,0,0),     // RED
    RGB(7,1,0),
    RGB(7,2,0),
    RGB(7,3,0),
    RGB(7,4,0),
    RGB(7,5,0),
    RGB(7,6,0),
    RGB(7,7,0),    // YELLOW
    RGB(6,7,0),
    RGB(5,7,0),
    RGB(4,7,0),
    RGB(3,7,0),
    RGB(2,7,0),
    RGB(1,7,0),
    RGB(0,7,0),    // GREEN
    RGB(0,7,1),
    RGB(0,7,1),
    RGB(0,7,2),
    RGB(0,7,2),
    RGB(0,7,2),
    RGB(0,7,3),
    RGB(0,7,3),    // CYAN
    RGB(0,6,3),
    RGB(0,5,3),
    RGB(0,4,3),
    RGB(0,3,3),
    RGB(0,2,3),
    RGB(0,1,3),
    RGB(0,0,3),    // BLUE
  };
  while(1){
    int light;
    int rg_brightness;
    int b_brightness;
    float t;
    delay(1000);
    light=analogRead(APIN_PHOTORESISTOR);
    rg_brightness=light/100;
    if(rg_brightness>7)
      rg_brightness=7;
    if(rg_brightness<1)
      rg_brightness=1;
    b_brightness=3;
    if(rg_brightness<3)
      b_brightness=rg_brightness/2;
    if(!b_brightness)
      b_brightness=1;
    if(light<00){
      for(p=0;p<display.pixels;p++)
        display.frameBuff[p]=BLACK;
      display.frameBuff[k]=1;
      k++;
      if(k>display.pixels)
        k=0;
      display.show();
      continue;
    };
    d=rtc.getDate();
    printTime(&display, rg_brightness, d.hour, d.minute, d.second);
    if(light>50){
      temperature=thermistor.read();
      display.printChar4p((char)(48+((int)temperature)/10), RGB(rg_brightness,0,0), 8, 0);
      display.printChar4p((char)(48+((int)temperature)%10), RGB(0,rg_brightness,0), 12, 0);
      display.printChar4p((char)(48+((int)(temperature*10)-10*(int)temperature)), RGB(0,0,b_brightness), 8, 4);
    }
    if(rg_brightness>2){
#define TEMP_MIN 15
#define TEMP_MAX 28.5
#define TEMP_RANGE (TEMP_MAX-TEMP_MIN)
#define TEMP_BAR_RANGE (temperatureBarSize-display.height)
      t=temperature;
      if(t<TEMP_MIN)
        t=TEMP_MIN;
      if(t>TEMP_MAX)
        t=TEMP_MAX;
      t-=TEMP_MIN;
      t=t/TEMP_RANGE;
      /*
      for(byte y=0;y<display.height;y++)
       display.paintPixel(temperatureBar[temperatureBarSize - int(t*float(TEMP_BAR_RANGE))   +   y   -8    ], 15, y);
       for(byte y=0;y<display.height;y++)
       display.paintPixel(temperatureBar[temperatureBarSize - int(t*float(TEMP_BAR_RANGE))   +   y   -8    ], 7, y);
       */
      display.paintPixel(temperatureBar[temperatureBarSize - int(t*float(temperatureBarSize))  ], 11, 4);
      display.printChar4p('C', temperatureBar[temperatureBarSize - int(t*float(temperatureBarSize))  ], 12, 4);
    } 
    else{
      if(light>50){
        display.paintPixel(RGB(2,2,1), 11, 4);
        display.printChar4p('C', RGB(2,2,1), 12, 4);
      }
    }
    display.show();
  }
}




















































