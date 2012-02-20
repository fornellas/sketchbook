#include "Fire.h"
#include <SFRGBLEDMatrix.h>
#include "Button.h"

extern SFRGBLEDMatrix *display;
extern Button *button;

#define PIXMAP(x,y) *(pixmap+(display->width+2)*(y)+(x))

// Color palete
#define INCANDESCENT_LEN 30
Color incandescent(byte p){
  // WHITE
  if(p<4){
    return RGB(15, 15, 15-(p*5));
  }
  // YELLOW
  if(p<8){
    p-=4;
    return RGB(15, 13-(p*2), 0);
  }
  // ORANGE
  if(p<15){
    p-=8;
    return RGB(15, 7-(p+1), 0);
  }
  // RED
  if(p<27){
    p-=15;
    return RGB(15-p, 0, 0);
  }
  // BLACK
  return RGB(0, 15/8, 15/2);
}

void Fire::enter() {
  display->gamma(true); 
  // Allocate pixmap
  pixmap=(byte *)malloc(size_t(sizeof(byte)*(display->width+2)*(display->height+2)));
  for(byte x=0;x<display->width+2;x++)
    for(byte y=0;y<display->height+2;y++)
      PIXMAP(x,y)=0;
  randomSeed(millis());
}

void Fire::loop() {
  static byte intensity;

  if(button->state(A))
    intensity=25;
  else
    intensity=10;

  if(button->state(B))
    intensity=0;

  // Update the row below visible screen with radom data
  for(byte x=4,y=display->height+1;x<display->width-2;x++){
    if(intensity){
      PIXMAP(x,y)=random(INCANDESCENT_LEN-1)+intensity;
      if(PIXMAP(x,y)>=INCANDESCENT_LEN)
        PIXMAP(x,y)=INCANDESCENT_LEN-1;
    }
    else
      PIXMAP(x,y)=0;
  }

  // Propagate the fire, by averaging each pixel with its surroundings
  for(int y=display->height;y>0;y--)
    for(byte x=1;x<display->width+1;x++)
      PIXMAP(x,y)=
        (
      PIXMAP(x,   y-1)+
        PIXMAP(x-1, y  )+
        PIXMAP(x+1, y  )+
        PIXMAP(x-1, y+1)+
        PIXMAP(x,   y+1)+
        PIXMAP(x+1, y+1)
        )/6;

  // Paint
  for(byte x=0;x<display->width;x++)
    for(byte y=0;y<display->height;y++)
      display->paintPixel(
      incandescent(INCANDESCENT_LEN-PIXMAP(x+1,y+1)),
      x, y);
  // And display
  display->show();
  delay(20);
}

void Fire::exit() {
  free(pixmap);
}

















