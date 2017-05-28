#include <Wire.h>

#include <Time.h>
#include <DS1307.h>
#define TIMEZONE 1
DS1307 *time;

#include <Adafruit_NeoPixel.h>
Adafruit_NeoPixel strip = Adafruit_NeoPixel(76, 6, NEO_GRB + NEO_KHZ800);

void updateTime(){
  btime_t bt;

  bt.year=2017;
  bt.mon=MAY;
  bt.mday=27;
  bt.hour=14;
  bt.min=25;
  bt.sec=10;
  bt.wday=SATURDAY;
  bt.zone=TIMEZONE;
  time->mktime(bt);
  time->saveToRTC();  
}

void setup(){
  Serial.begin(115200);
  Wire.begin();
  time=new DS1307(TIMEZONE);
//  updateTime();
  strip.begin();
  strip.show();
}

void serialPrintTime(btime_t bt){
  Serial.print(bt.year);
  Serial.print("-");
  Serial.print(bt.mon);
  Serial.print("-");
  Serial.print(bt.mday);
  Serial.print(" ");
  Serial.print(bt.hour);
  Serial.print(":");
  Serial.print(bt.min);
  Serial.print(":");
  Serial.print(bt.sec);
  Serial.print(" TZ(");
  Serial.print(bt.zone);
  Serial.println(")");
}

void clearStrip(){
  for(uint16_t i=0;i<strip.numPixels();i++){
    strip.setPixelColor(i, strip.Color(0, 0, 0));
  }
}

static uint8_t rainbow=0;

uint32_t getPixelColor(uint16_t i){
  byte WheelPos;
  WheelPos = (rainbow + i) & 255;
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}

void litPixels(uint16_t start, uint16_t finish){
  for(uint16_t i=start;i<=finish;i++){
    strip.setPixelColor(i, getPixelColor(i));
  }
}

void updateHour(uint8_t hour){
  switch(hour){
    case 1:
      litPixels(40, 42); // ONE
      break;
    case 2:
      litPixels(34, 36); // TWO
      break;
    case 3:
      litPixels(43, 46); // THREE
      break;
    case 4:
      litPixels(31, 33); // FOUR
      break;
    case 5:
      litPixels(29, 30); // FIVE
      break;
    case 6:
      litPixels(19, 20); // SIX
      break;
    case 7:
      litPixels(21, 24); // SEVEN
      break;
    case 8:
      litPixels(25, 28); // EIGHT
      break;
    case 9:
      litPixels(16, 18); // NINE
      break;
    case 10:
      litPixels(14, 15); // TEN
      break;
    case 11:
      litPixels(10, 13); // ELEVEN
      break;
    case 12:
    case 0:
      litPixels(0, 4); // TWELVE 
      break;
  }
}

void updatePixels(uint8_t hour, uint8_t min){
  clearStrip();
  litPixels(74, 76); // IT'S

  if(0==hour){
    hour = 12;
  }else{
    hour = hour%12;
  }

  if(min < 35){
    updateHour(hour);
    if(min >= 0 && min <5){
      litPixels(5, 9); // O'CLOCK
    }else {
      if(min < 30) {
        if(min >= 5 && min <10){
          litPixels(55, 57); // FIVE
          litPixels(49, 54); // MINUTES
        }else if(min >= 10 && min <15){
          litPixels(69, 70); // TEN
          litPixels(49, 54); // MINUTES
        }else if(min >= 15 && min <20){
          litPixels(58, 63); // QUARTER
        }else if(min >= 20 && min <25){
          litPixels(64, 68); // TWENTY
          litPixels(49, 54); // MINUTES
        }else if(min >= 25 && min <30){
          litPixels(64, 68); // TWENTY
          litPixels(55, 57); // FIVE
          litPixels(49, 54); // MINUTES
        }
      }else if(min >=30 && min < 35){
        litPixels(71, 73); // HALF
      }
      litPixels(37, 39); // PAST
    }
  }else{
    if(hour<12){
      updateHour(hour+1);
    }else{
      updateHour(1);
    }
    if(min >= 35 && min < 40){
      litPixels(64, 68); // TWENTY
      litPixels(55, 57); // FIVE
      litPixels(49, 54); // MINUTES
    }else if(min >= 40 && min < 45){
      litPixels(64, 68); // TWENTY
      litPixels(49, 54); // MINUTES
    }else if(min >= 45 && min < 50){
      litPixels(58, 63); // QUARTER
    }else if(min >= 50 && min < 55){
      litPixels(69, 70); // TEN
      litPixels(49, 54); // MINUTES
    }else if(min >= 55 && min < 60){
      litPixels(55, 57); // FIVE
      litPixels(49, 54); // MINUTES
    }
    litPixels(47, 48); // TO
  }

  strip.show();
  rainbow += 1;
}

void demo(){
  while(true) {
    for(uint8_t hour=1;hour<=12;hour++){
      for(uint8_t min=0;min<60;min++){
        updatePixels(hour, min);
        rainbow += 1;
        delay(500);
      }
    }
  }
}

void loop(){
  btime_t bt;

//  demo();
  
  time->loadFromRTC();
  bt=time->getBTime();
  serialPrintTime(bt);

  updatePixels(bt.hour, bt.min);
 
  delay(1000);
}
