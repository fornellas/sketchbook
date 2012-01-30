#include <SFRGBLEDMatrix.h>
#include <SPI.h>

#define PIN_MATRIX_SS   10

#define DISP_COUNT 1

void setup(){
  SPI.begin();
};

void loop(){
  SFRGBLEDMatrix display(SIDE, DISP_COUNT, PIN_MATRIX_SS);
  display.config();

  while(1){
    for(byte j=0;j<4;j++){
      for(byte x=0;x<display.width;x++){
        for(byte y=0;y<DISP_LEN;y++){
          display.frameBuff[y*display.width+x]=RGB(x,y,j);
        }
      }
      display.show();
      delay(300);
    }
    for(byte j=2;j>0;j--){
      for(byte x=0;x<display.width;x++){
        for(byte y=0;y<DISP_LEN;y++){
          display.frameBuff[y*display.width+x]=RGB(x,y,j);
        }
      }
      display.show();
      delay(300);
    }
  }
}








