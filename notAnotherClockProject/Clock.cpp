#include "Clock.h"
#include "pins.h"

#include <EEPROM.h>
#include <avr/pgmspace.h>
#include "facilities.h"

#define BUFF_LCD_MSG 18

// LCD

void
Clock::lcdInfo(){
  char buff[BUFF_LCD_MSG];
  byte t;

  // read values
  dht22->loadFromSensor();
  switch(dht22->error()){
    case DHT_ERROR_NONE:
    case DHT_ERROR_TOOQUICK:
      break;
    default:
      // FIXME better error reporting
      ledMatrix->fill(RED);
      ledMatrix->show();
      while(1);
      break;
  }
  bmp085->loadFromSensor();
  humidityInside.loadFromSensor();
  // LCD
  lcd->firstPage();
  do {
    byte x;
    byte y;
    // Temperature names
    y=-1;
    lcd->setFont(u8g_font_5x7);
    lcd->setFontPosTop();
    x=lcd->getWidth()/4-lcd->getStrWidthP(U8G_PSTR("Inside"))/2;
    lcd->drawStrP(x, y, U8G_PSTR("Inside"));
    x=lcd->getWidth()-lcd->getStrWidthP(U8G_PSTR("Outside"));
    x=lcd->getWidth()*3/4-lcd->getStrWidthP(U8G_PSTR("Outside"))/2;
    lcd->drawStrP(x, y, U8G_PSTR("Outside"));
    // Temperature inside
    x=0;
    y=lcd->getFontAscent();
    lcd->setFont(u8g_font_fub14);
    lcd->setFontPosTop();
    lcd->drawStr(x, y, dtostrf(temperatureInside.getC(), -2, 1, buff));
    x+=lcd->getStrWidth(dtostrf(temperatureInside.getC(), -2, 1, buff));
    buff[0]=176;
    buff[1]='C';
    buff[2]='\0';
    lcd->drawStr(x, y, buff);
    // Temperature outside
    dtostrf(temperatureOutside.getC(), -2, 1, buff);
    t=strlen(buff);
    buff[t++]=176;
    buff[t++]='C';
    buff[t]='\0';
    x=lcd->getWidth()-lcd->getStrWidth(buff);
    lcd->drawStr(x, y, buff);
    // Humidity inside
    y+=lcd->getFontAscent()+1;
    ltoa(humidityInside.getRH(), buff, 10);
    t=strlen(buff);
    buff[t++]='%';
    buff[t]='\0';
    x=lcd->getWidth()/4-lcd->getStrWidth(buff)/2;
    lcd->drawStr(x, y, buff);
    // Humidity outside
    ltoa(humidityOutside.getRH(), buff, 10);
    t=strlen(buff);
    buff[t++]='%';
    buff[t]='\0';
    x=lcd->getWidth()*3/4-lcd->getStrWidth(buff)/2;
    lcd->drawStr(x, y, buff);
    // Pressure
    ltoa(pressure.readHPa(), buff, 10);
    t=strlen(buff);
    buff[t++]='h';
    buff[t++]='P';
    buff[t++]='a';
    buff[t]='\0';
    y+=lcd->getFontAscent()+1;
    lcd->setFont(u8g_font_7x13B);
    lcd->setFontPosTop();
    x=lcd->getWidth()/2-lcd->getStrWidth(buff)/2;
    lcd->drawStr(x, y, buff);
    // Week day
    y+=lcd->getFontAscent();
    lcd->setFont(u8g_font_6x12);
    lcd->setFontPosTop();
    x=lcd->getWidth()/2-lcd->getStrWidthP((u8g_pgm_uint8_t *)time->getWeekDayName())/2;
    lcd->drawStrP(x, y, (const u8g_pgm_uint8_t*)time->getWeekDayName());
    // Date
    strcpy_P(buff, time->getMonthName());
    t=strlen(buff);
    buff[t++]=' ';
    itoa(date.mday, buff+t, 10);
    t=strlen(buff);
    buff[t++]=' ';
    itoa(date.year, buff+t, 10);
    y+=lcd->getFontAscent()+2;
    x=lcd->getWidth()/2-lcd->getStrWidth(buff)/2;
    lcd->drawStr(x, y, buff);
  } 
  while( lcd->nextPage() );
}

// Digital

void Clock::showDigital(){
  int xOffset;
  int yOffset;
  xOffset=(ledMatrix->width>>1)-8;
  yOffset=(ledMatrix->height>>1)-11/2-1;
  ledMatrix->print(RGB(value,0,0), xOffset+0, yOffset+0, 5, 48+(date.hour/10));
  ledMatrix->print(RGB(value,0,0), xOffset+4, yOffset+0, 5, 48+(date.hour%10));
  ledMatrix->print(RGB(0,value,0), xOffset+8, yOffset+0, 5, 48+(date.min/10));
  ledMatrix->print(RGB(0,value,0), xOffset+12, yOffset+0, 5, 48+(date.min%10));
  ledMatrix->print(RGB(0,0,value), xOffset+4, yOffset+7, 5, 48+(date.sec/10));
  ledMatrix->print(RGB(0,0,value), xOffset+8, yOffset+7, 5, 48+(date.sec%10));
}

// Binary

void Clock::drawBCDdigit(byte value, int x, int y, byte digit){
  if(digit&(1<<3))
    ledMatrix->box(RGB(value, value, value), x, y, x+1, y+1);
  if(digit&(1<<2))
    ledMatrix->box(RGB(0, 0, value), x, y+3, x+1, y+1+3);
  if(digit&(1<<1))
    ledMatrix->box(RGB(0, value, 0), x, y+6, x+1, y+1+6);
  if(digit&1)
    ledMatrix->box(RGB(value, 0, 0), x, y+9, x+1, y+1+9);
}

void Clock::showBinary(){
  drawBCDdigit(value, 2, 3, date.hour/10);
  drawBCDdigit(value, 5, 3, date.hour%10);
  drawBCDdigit(value, 10, 3, date.min/10);
  drawBCDdigit(value, 13, 3, date.min%10);
  drawBCDdigit(value, 18, 3, date.sec/10); 
  drawBCDdigit(value, 21, 3, date.sec%10); 
  ledMatrix->paintPixel(RGB(value, value, value), 8, 6);
  ledMatrix->paintPixel(RGB(value, value, value), 8, 10);
  ledMatrix->paintPixel(RGB(value, value, value), 16, 6);
  ledMatrix->paintPixel(RGB(value, value, value), 16, 10);
}

// DX Time

void Clock::showDX() {
  Color centerColor=RGB(value,value,value);
  Color hourColor=RGB(value,0,0);
  Color minuteColor5=RGB(value,value,0);
  Color minuteColor1=RGB(0,value,0);
  Color secondColor=RGB(0,0,value);
  int xOff;
  xOff=4;
  // Middle point
  ledMatrix->paintPixel(centerColor, xOff+7, 7);
  ledMatrix->paintPixel(centerColor, xOff+8, 7);
  ledMatrix->paintPixel(centerColor, xOff+7, 8);
  ledMatrix->paintPixel(centerColor, xOff+8, 8);
  // Hour
  int hour;
  hour=date.hour;
  if(hour>=13)
    hour-=12;
  byte hx1[12] = {
    11,14,15,14,11,7,3,1,0,1,3,7                                                                                                            };
  byte hy1[12] = {
    1,3,7,11,14,15,14,11,7,3,1,0                                                                                                            };
  byte hx2[12] = {
    12,14,15,14,12,8,4,1,0,1,4,8                                                                                                            };
  byte hy2[12] = {
    1,4,8,12,14,15,14,12,8,4,1,0                                                                                                            };
  for(byte h=1;h<13;h++){
    if(hour>=h||hour==0) {
      ledMatrix->paintPixel(hourColor, xOff+hx1[h-1], hy1[h-1]);
      ledMatrix->paintPixel(hourColor, xOff+hx2[h-1], hy2[h-1]);
    }
  }

  // minute
  byte mx1[12] = {
    10,12,13,12,10,7,4,3,2,3,4,7                                                                                                            };
  byte my1[12] = {
    3,4,7,10,12,13,12,10,7,4,3,2                                                                                                            };
  byte mx2[12] = {
    11,12,13,12,11,8,5,3,2,3,5,8                                                                                                            };
  byte my2[12] = {
    3,5,8,11,12,13,12,11,8,5,3,2                                                                                                            };
  for(byte m=5;m<=60;m+=5){
    if(date.min>=m){
      ledMatrix->paintPixel(minuteColor5, xOff+mx1[m/5-1], +my1[m/5-1]);
      ledMatrix->paintPixel(minuteColor5, xOff+mx2[m/5-1], +my2[m/5-1]);
    }
  }
  if(date.min%5>=1){
    ledMatrix->paintPixel(minuteColor1, xOff+7, 4);
    ledMatrix->paintPixel(minuteColor1, xOff+8, 4);
  }
  if(date.min%5>=2){
    ledMatrix->paintPixel(minuteColor1, xOff+11, 7);
    ledMatrix->paintPixel(minuteColor1, xOff+11, 8);
  }
  if(date.min%5>=3){
    ledMatrix->paintPixel(minuteColor1, xOff+7, 11);
    ledMatrix->paintPixel(minuteColor1, xOff+8, 11);
  }
  if(date.min%5>=4){
    ledMatrix->paintPixel(minuteColor1, xOff+4, 7);
    ledMatrix->paintPixel(minuteColor1, xOff+4, 8);
  }
  // Second
  if(date.sec%5>=1){
    ledMatrix->paintPixel(secondColor, xOff+7, 6);
    ledMatrix->paintPixel(secondColor, xOff+8, 6);
  }
  if(date.sec%5>=2){
    ledMatrix->paintPixel(secondColor, xOff+9, 7);
    ledMatrix->paintPixel(secondColor, xOff+9, 8);
  }
  if(date.sec%5>=3){
    ledMatrix->paintPixel(secondColor, xOff+7, 9);
    ledMatrix->paintPixel(secondColor, xOff+8, 9);
  }
  if(date.sec%5>=4){
    ledMatrix->paintPixel(secondColor, xOff+6, 7);
    ledMatrix->paintPixel(secondColor, xOff+6, 8);
  }
}

// Big digit

void Clock::drawBigDigit(Color color, int x, int y, byte digit){
  switch(digit){
  case 0:
    ledMatrix->line(color, x+2, y+4, x+2, y+9);
    break;
  case 1:
    ledMatrix->line(color, x+2, y, x+2, y+ledMatrix->height-2);
    break;
  case 2:
    ledMatrix->line(color, x, y+4, x+2, y+4);
    ledMatrix->line(color, x+2, y+9, x+4, y+9);
    break;
  case 3:
    ledMatrix->line(color, x, y+4, x+2, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    break;
  case 4:
    ledMatrix->line(color, x+2, y, x+2, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    ledMatrix->line(color, x+2, y+10, x+2, y+13);
    break;
  case 5:
    ledMatrix->line(color, x+2, y+4, x+4, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    break;
  case 6:
    ledMatrix->line(color, x+2, y+4, x+4, y+4);
    ledMatrix->paintPixel(color, x+2, y+9);
    break;
  case 7:
    ledMatrix->line(color, x, y+4, x+2, y+4);
    ledMatrix->line(color, x+2, y+4, x+2, y+13);
    break;
  case 8:
    ledMatrix->paintPixel(color, x+2, y+4);
    ledMatrix->paintPixel(color, x+2, y+9);
    break;
  case 9:
    ledMatrix->paintPixel(color, x+2, y+4);
    ledMatrix->line(color, x, y+9, x+2, y+9);
    ledMatrix->line(color, x+2, y+10, x+2, y+13);
    break;
  }
}

void Clock::showBigDigit(){
  ledMatrix->line(RGB(0, 0, value), 0, 0, ledMatrix->width-1, 0);
  ledMatrix->line(RGB(0, 0, value), 0, ledMatrix->height-1, ledMatrix->width-1, ledMatrix->height-1);
  ledMatrix->line(RGB(0, 0, value), 5, 1, 5, ledMatrix->height-2);
  ledMatrix->line(RGB(0, 0, value), 18, 1, 18, ledMatrix->height-2);
  ledMatrix->box(RGB(0, 0, value), ledMatrix->width/2-1, 1, ledMatrix->width/2, ledMatrix->height-2);
  drawBigDigit(RGB(0, 0, value), 0, 1, date.hour/10);
  drawBigDigit(RGB(0, 0, value), 6, 1, date.hour%10);
  drawBigDigit(RGB(0, 0, value), 13, 1, date.min/10);
  drawBigDigit(RGB(0, 0, value), 19, 1, date.min%10);
}

// main stuff

Clock::Clock():
Mode(PSTR("Clock")){
  lastLCDUpdate=millis();
  lastLCDUpdate=0;
  time=new DS1307(TIMEZONE);
}

void Clock::loop(){
  unsigned long now;

  // intensity
  value=light->read(MAX_C);
  value+=3;
  if(value>MAX_C) //tunning
    value=MAX_C;
  value=V_GAMMA(value);

  // Date
  time->loadFromRTC();
  date=time->getBTime();

  // LCD
  if((now=millis())-lastLCDUpdate>1000){
    lastLCDUpdate=now;
    lcdInfo();
  }

  // LED Matrix
  ledMatrix->fill(BLACK);

  // change mode if MODE button is pressed
  if(button.pressed(BUTTON_A)) {
    EEPROM.write(EEPROM_CLOCK_MODE, EEPROM.read(EEPROM_CLOCK_MODE)+1);
  }

  switch(EEPROM.read(EEPROM_CLOCK_MODE)){
    // Digital
  case 0:
    showDigital();
    break;
    // Binary
  case 1:
    showBinary();
    break;
    // showDX
  case 2:
    showDX();
    break;
    // Big digit, inspired by http://www.tokyoflash.com/en/watches/kisai/stencil/
  case 3:
    showBigDigit();
    break;
  default:
    EEPROM.write(EEPROM_CLOCK_MODE, 0);
    return;
  }
  ledMatrix->show();
}

Clock::~Clock(){
  delete time;
}
