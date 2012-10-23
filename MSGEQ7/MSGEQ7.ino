#include <SPI.h>
#include <SFRGBLEDMatrix.h>

#define DC_OUT A0
#define STROBE 2
#define RESET 3
#define BANDS 7

#define PIN_MATRIX_SS 10
#define DISP_HORIZ 3
#define DISP_VERT 2

// 600mV output offset
#define DC_OFFSET (0.6/5.0*1023.0)

SFRGBLEDMatrix *display;

#define SPECTRUM_LEN 42
Color spectrum[]={
  // RED
  RGB(7, 1, 0), RGB(7, 2, 0), RGB(7, 3, 0), RGB(7, 4, 0), RGB(7, 5, 0), RGB(7, 6, 0), RGB(7, 7, 0), 
  // YELLOW
  RGB(6, 7, 0), RGB(5, 7, 0), RGB(4, 7, 0), RGB(3, 7, 0), RGB(2, 7, 0), RGB(1, 7, 0), RGB(0, 7, 0), 
  // GREEN
  RGB(0, 7, 1), RGB(0, 7, 1), RGB(0, 7, 2), RGB(0, 7, 2), RGB(0, 7, 2), RGB(0, 7, 3), RGB(0, 7, 3), 
  // CYAN
  RGB(0, 6, 3), RGB(0, 5, 3), RGB(0, 4, 3), RGB(0, 3, 3), RGB(0, 2, 3), RGB(0, 1, 3), RGB(0, 0, 3), 
  // BLUE
  RGB(1, 0, 3), RGB(2, 0, 3), RGB(3, 0, 3), RGB(4, 0, 3), RGB(5, 0, 3), RGB(6, 0, 3), RGB(7, 0, 3), 
  // MAGENTA
  RGB(7, 0, 2), RGB(7, 0, 2), RGB(7, 0, 1), RGB(7, 0, 1), RGB(7, 0, 1), RGB(7, 0, 0), RGB(7, 0, 0), 
};

int filter=1;

void
setup(){
  SPI.begin();

  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, DISP_HORIZ, DISP_VERT);
  display->gamma(false);
  pinMode(STROBE, OUTPUT);
  digitalWrite(STROBE, LOW);
  pinMode(RESET, OUTPUT);
  digitalWrite(RESET, LOW);  

  // Reset
  digitalWrite(RESET, HIGH);
  // minimum pulse width 100ns
  delayMicroseconds(1);
  digitalWrite(RESET, LOW);
  digitalWrite(STROBE, HIGH);
  // minimum delay after: 72us
  delayMicroseconds(72);
  digitalWrite(STROBE, LOW);
}

void
loop(){
  int value[BANDS];
  int c, v;

  // Read value
  for(c=0;c<BANDS;c++){
    // minimum delay before read: 36us
    delayMicroseconds(36);
    value[c]=analogRead(DC_OUT);
    // minimum strobe to strobe: 72us
    delayMicroseconds(18);
    digitalWrite(STROBE, HIGH);
    // minimum strobe pulse width: 18us
    delayMicroseconds(18);
    digitalWrite(STROBE, LOW);
  }
  display->fill(BLACK);
  for(c=0;c<BANDS;c++){
    if(value[c]<DC_OFFSET)
      value[c]=0;
    else
      value[c]-=DC_OFFSET;
    int clear=1;
    for(v=0;v<DISP_VERT*8;v++){
      if((8.0*DISP_VERT)*double(value[c])/(1023.0-DC_OFFSET)>v){
        /*        display->paintPixel(RGB(0,0,v+1),c*3+2,DISP_VERT*8-v-1);        
         display->paintPixel(RGB(0,0,v+1),c*3+3,DISP_VERT*8-v-1);
         }
         else{
         display->paintPixel(RGB((v>>1)+1,0,0),c*3+2,DISP_VERT*8-v-1);        
         display->paintPixel(RGB((v>>1)+1,0,0),c*3+3,DISP_VERT*8-v-1);
         break;
         */
        clear=0;
        if(!filter){
          display->paintPixel(spectrum[int(float(v)/15.0*float(SPECTRUM_LEN-1))],c*3+2,DISP_VERT*8-v-1);        
          display->paintPixel(spectrum[int(float(v)/15.0*float(SPECTRUM_LEN-1))],c*3+3,DISP_VERT*8-v-1);
        }
      }
    }
    if(clear)
      filter=1;
    else
      filter=0;
  }
  display->show(); 
}































