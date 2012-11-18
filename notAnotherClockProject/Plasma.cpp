#include "Plasma.h"
#include "EEPROM_addr.h"
#include "Button.h"
#include <EEPROM.h>

extern SFRGBLEDMatrix *ledMatrix;

#define PIXMAP_WIDTH ((ledMatrix->width>>3)+3)
#define PIXMAP_HEIGHT ((ledMatrix->height>>3)+3)
#define PIXMAP_DEPTH 150.0

// Bicubic interpolation code based on code found at:
// http://www.paulinternet.nl/?page=bicubic
double Plasma::cubicInterpolate (double *p, double x) {
  return *(p+1) + 0.5 * x*(*(p+2) - *p + x*(2.0**(p+0) - 5.0**(p+1) + 4.0**(p+2) - *(p+3) + x*(3.0*(*(p+1) - *(p+2)) + *(p+3) - *p)));
}

double Plasma::bicubicInterpolate (double *pixmap, uint8_t pixmapWidth, uint8_t pixmapHeight, uint8_t pointX, uint8_t pointY, double x, double y) {
  double arr[4];
  arr[0] = cubicInterpolate(pixmap+pixmapWidth*(pointY-1)+pointX-1, x);
  arr[1] = cubicInterpolate(pixmap+pixmapWidth*(pointY+0)+pointX-1, x);
  arr[2] = cubicInterpolate(pixmap+pixmapWidth*(pointY+1)+pointX-1, x);
  arr[3] = cubicInterpolate(pixmap+pixmapWidth*(pointY+2)+pointX-1, x);
  return cubicInterpolate(arr, y);
}

void Plasma::fillPlasma() {
  double *pixmap;
  int max, step=0;

  // progress bar
  ledMatrix->progressBarInit(WHITE);
  max=(int)PIXMAP_WIDTH+(int)ledMatrix->width;

  pixmap=(double *)malloc(sizeof(double)*(PIXMAP_WIDTH*PIXMAP_HEIGHT)); // FIXME Error reporting

  randomSeed(millis());
  for(byte x=0;x<PIXMAP_WIDTH;x++){
    ledMatrix->progressBarUpdate(BLUE, ++step, max);
    for(byte y=0;y<PIXMAP_HEIGHT;y++)
      *(pixmap+PIXMAP_WIDTH*y+x)=random(PIXMAP_DEPTH);
  }

  for(word x=0;x<ledMatrix->width;x++){
    ledMatrix->progressBarUpdate(BLUE, ++step, max);
    for(word y=0;y<ledMatrix->height;y++)
      *(plasma+y*ledMatrix->width+x)=bicubicInterpolate(
      pixmap,
      PIXMAP_WIDTH, 
      PIXMAP_HEIGHT,
      (x/8)+1, 
      (y/8)+1, 
      double(x%8)/8.0,
      double(y%8)/8.0
        );
  }

  free(pixmap);
}

void Plasma::pSpeedValidate(){
  if(pSpeed>6)
    pSpeed=6;
  if(pSpeed<-6)
    pSpeed=-6;
}

Plasma::Plasma():
Mode(PSTR("Plasma")){
  ledMatrix->gamma(true); 
  pSpeed=EEPROM.read(EEPROM_PLASMA_SPEED);
  pSpeedValidate();
  plasma=(byte *)malloc((size_t)(ledMatrix->pixels)*sizeof(byte)); // FIXME Error reporting
  fillPlasma();
}

void Plasma::loop(){
  boolean changeSpeed=false;

  // Update pimap
  if(button.pressed(BUTTON_A)){
    fillPlasma();
  }

  // Change speed
  if(button.pressed(BUTTON_B)){
    changeSpeed=true;
    pSpeed++;
  }
  if(button.pressed(BUTTON_C)){
    changeSpeed=true;
    pSpeed--;
  }

  if(changeSpeed){
    pSpeedValidate();
    // FIXME adjust text placement relative to screen size
    if(pSpeed>=0)
      ledMatrix->print(BLACK, 11, 6, 5, 48+pSpeed);
    else{
      ledMatrix->print(BLACK, 9, 6, 5, '-');
      ledMatrix->print(BLACK, 12, 6, 5, 48+pSpeed*-1);
    }
    ledMatrix->show();
    delay(200);
    EEPROM.write(EEPROM_PLASMA_SPEED, pSpeed);
  }

  // Update pixmap
  for(byte x=0;x<ledMatrix->width;x++)
    for(byte y=0;y<ledMatrix->height;y++)
      *(plasma+y*ledMatrix->width+x)+=pSpeed;

  // Paint display
  for(word x=0;x<ledMatrix->width;x++)
    for(word y=0;y<ledMatrix->height;y++)
      ledMatrix->paintPixel(
      ledMatrix->spectrum(
      *(plasma+y*word(ledMatrix->width)+x),
      255),
      x,
      y);

  // Show
  ledMatrix->show();
}

Plasma::~Plasma(){
  free(plasma);
}





