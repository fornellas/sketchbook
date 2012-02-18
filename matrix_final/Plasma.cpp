#include "Plasma.h"
#include <SFRGBLEDMatrix.h>
#include "Button.h"
#include <EEPROM.h>
#include "EEPROM.h"

extern SFRGBLEDMatrix *display;
extern Button *button;

#define SPECTRUM_LEN 90

Color spectrum(byte p){
  // RED
  if(p<15){
    return RGB(15, p+1, 0);
  }
  // YELLOW
  if(p<30){
    p-=15;
    return RGB(14-p, 15, 0);
  }
  // GREEN
  if(p<45){
    p-=30;
    return RGB(0, 15, p+1);
  }
  // CYAN
  if(p<60){
    p-=45;
    return RGB(0, 14-p, 15);
  }
  // BLUE
  if(p<75){
    p-=60;
    return RGB(p+1, 0, 15);
  }
  // MAGENTA
  if(p<90){
    p-=75;
    return RGB(15, 0, 14-p);
  }
}

// http://www.paulinternet.nl/?page=bicubic
double cubicInterpolate (double p[4], double x) {
  return p[1] + 0.5 * x*(p[2] - p[0] + x*(2.0*p[0] - 5.0*p[1] + 4.0*p[2] - p[3] + x*(3.0*(p[1] - p[2]) + p[3] - p[0])));
}
double bicubicInterpolate (double p[4][4], double x, double y) {
  double arr[4];
  arr[0] = cubicInterpolate(p[0], y);
  arr[1] = cubicInterpolate(p[1], y);
  arr[2] = cubicInterpolate(p[2], y);
  arr[3] = cubicInterpolate(p[3], y);
  return cubicInterpolate(arr, x);
}

#define ORIGIN_WIDTH 4
#define ORIGIN_HEIGHT 4
#define ORIGIN_MAX 64.0

void Plasma::fillPlasma() {
  double origin[ORIGIN_WIDTH][ORIGIN_HEIGHT];

  randomSeed(millis());
  for(byte x=0;x<ORIGIN_WIDTH;x++)
    for(byte y=0;y<ORIGIN_HEIGHT;y++)
      origin[x][y]=random(ORIGIN_MAX);

  for(byte x=0;x<ORIGIN_WIDTH;x++){
    for(byte y=0;y<ORIGIN_HEIGHT;y++){
      Serial.print(origin[x][y]);
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("");

  for(double x=0;x<display->width;x++)
    for(double y=0;y<display->height;y++)
      *(plasma+word(y)*display->width+word(x))=bicubicInterpolate(origin, x/(double)display->width, y/(double)display->height);

  for(byte x=0;x<display->width;x++){
    for(byte y=0;y<display->height;y++){
      Serial.print(*(plasma+y*display->width+x));
      Serial.print(" ");
    }
    Serial.println("");
  }
  Serial.println("");

}


void Plasma::pSpeedValidate(){
  if(pSpeed==0)
    pSpeed++;
  if(pSpeed>6)
    pSpeed=-6;
  if(pSpeed<-6)
    pSpeed=-6;
}

void Plasma::loop() {
  if(button->released(A)){
    display->clear();
    display->printString4p("BUSY", WHITE, 0, 6);
    display->show();
    fillPlasma();
  }

  if(button->released(B)) {
    pSpeed++;
    pSpeedValidate();
    if(pSpeed>=0)
      display->printChar4p(48+pSpeed, BLACK, 7, 6);
    else{
      display->printChar4p('-', BLACK, 5, 6);
      display->printChar4p(48+pSpeed*-1, BLACK, 8, 6);
    }
    display->show();
    delay(200);
    EEPROM.write(EEPROM_PLASMA_SPEED, pSpeed);
  }

  for(byte x=0;x<display->width;x++)
    for(byte y=0;y<display->height;y++)
      *(plasma+y*display->width+x)+=pSpeed;

  for(word x=0;x<display->width;x++)
    for(word y=0;y<display->height;y++)
      //      display->paintPixel(word(double(*(plasma+y*word(display->width)+x)/255.0*4096.0)), x, y);
      display->paintPixel(spectrum(
      byte(
      double(
      *(plasma+y*word(display->width)+x)
        ) / 255.0 * double(SPECTRUM_LEN-1)
        ))
        , x, y);

  display->show();
}

void Plasma::enter() {
  display->printString4p("BUSY", WHITE, 0, 6);
  display->show();
  plasma=(byte *)malloc((size_t)(display->pixels)*sizeof(byte));
  fillPlasma();
}

void Plasma::exit() {
  display->fill(BLACK);
  display->show();
  free(plasma);
}


































