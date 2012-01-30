#include <SFRGBLEDMatrix.h>
#include <SPI.h>

#define PIN_MATRIX_SS   10

#define DISP_COUNT 2

void setup(){
  SPI.begin();
}
#define VLEN 42
void loop(){
  word x,y,j;

  byte v[]={
    // RED
    RGB(7,1,0),
    RGB(7,2,0),
    RGB(7,3,0),
    RGB(7,4,0),
    RGB(7,5,0),
    RGB(7,6,0),
    RGB(7,7,0),
    // YELLOW
    RGB(6,7,0),
    RGB(5,7,0),
    RGB(4,7,0),
    RGB(3,7,0),
    RGB(2,7,0),
    RGB(1,7,0),
    RGB(0,7,0),
    // GREEN
    RGB(0,7,1),
    RGB(0,7,1),
    RGB(0,7,2),
    RGB(0,7,2),
    RGB(0,7,2),
    RGB(0,7,3),
    RGB(0,7,3),
    // CYAN
    RGB(0,6,3),
    RGB(0,5,3),
    RGB(0,4,3),
    RGB(0,3,3),
    RGB(0,2,3),
    RGB(0,1,3),
    RGB(0,0,3),
    // BLUE
    RGB(1,0,3),
    RGB(2,0,3),
    RGB(3,0,3),
    RGB(4,0,3),
    RGB(5,0,3),
    RGB(6,0,3),
    RGB(7,0,3),
    // MAGENTA
    RGB(7,0,2),
    RGB(7,0,2),
    RGB(7,0,1),
    RGB(7,0,1),
    RGB(7,0,1),
    RGB(7,0,0),
    RGB(7,0,0),
  };

  SFRGBLEDMatrix display(SIDE, DISP_COUNT, PIN_MATRIX_SS);
  display.config();
  while(1)
    for(j=0;j<=VLEN;j++){
      for(x=0;x<display.width;x++){
        for(byte y=0;y<DISP_LEN;y++){
          display.frameBuff[y*display.width+x]=v[(y+j+VLEN)%VLEN];
        }
      }
      display.show();
      delay(100);
    }
}
























