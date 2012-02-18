extern "C" {
  #include <inttypes.h>
  #include <stdlib.h>
  #include <math.h>
}

#include <Arduino.h>
#include <SFRGBLEDMatrix.h>
#include <SPI.h>

#define DISPLAY_PIXELS 64
#define BITS_PER_COLOR 4
#define DISPLAY_BUFFER_SIZE (DISPLAY_PIXELS*BITS_PER_COLOR*3>>3)

prog_uint16_t coffset_4p[] PROGMEM={
  0, 1, 2, 5, 10, 13, 17, 21, 22, 24, 26, 31, 34, 36, 38, 39, 43, 46, 49, 52, 55, 58, 61, 64, 67, 70, 73, 74, 76, 78, 80, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 115, 118, 122, 125, 130, 134, 138, 141, 145, 148, 151, 154, 158, 162, 167, 170, 173, 177, 179, 183, 185, 188, 191, 193, 196, 199, 202, 205, 208, 210, 212, 215, 216, 219, 222, 224, 229, 232, 235, 238, 241, 243, 246, 249, 252, 255, 260, 263, 266, 269, 272, 273, 276, 280};
prog_uchar line0_4p[] PROGMEM={
  0x6a, 0x9c, 0xb5, 0xaa, 0x0, 0x29, 0x6c, 0x7b, 0xff, 0x90, 0x33, 0x59, 0xef, 0xdd, 0xe6, 0x64, 0x65, 0xb3, 0x67, 0xe6, 0x63, 0x6f, 0xf1, 0xa1, 0x8, 0x8, 0x79, 0x12, 0x0, 0x19, 0x1, 0x0, 0x1, 0x7b, 0xe0};
prog_uchar line1_4p[] PROGMEM={
  0x6f, 0xf1, 0x26, 0x5c, 0x80, 0x57, 0x17, 0x64, 0x3e, 0xc7, 0x8d, 0xbe, 0x5d, 0x25, 0x46, 0xa6, 0xf6, 0x6c, 0xd8, 0xa6, 0x6a, 0xa9, 0x48, 0xd0, 0xbc, 0xdb, 0xbc, 0x36, 0xd6, 0x56, 0xef, 0xdb, 0x1b, 0xce, 0xb5};
prog_uchar line2_4p[] PROGMEM={
  0x7, 0xda, 0x4a, 0x5d, 0xdc, 0x95, 0x43, 0xcf, 0x57, 0x98, 0x44, 0xf6, 0x59, 0xaf, 0x57, 0xa5, 0x6e, 0x75, 0xe2, 0xa5, 0x77, 0x52, 0x44, 0x80, 0x5b, 0x2e, 0xdb, 0xba, 0xad, 0xbb, 0xc9, 0x5b, 0x54, 0x52, 0xaa};
prog_uchar line3_4p[] PROGMEM={
  0x42, 0xb4, 0xb1, 0xaa, 0xa3, 0xb, 0xfc, 0x77, 0x9c, 0xe7, 0x93, 0xb9, 0xef, 0x1d, 0xea, 0x7c, 0x65, 0xa3, 0xdc, 0x98, 0xa3, 0x57, 0xe3, 0x8e, 0x3c, 0xdb, 0xbb, 0x57, 0xad, 0x51, 0xd9, 0x34, 0xab, 0xbb, 0xe0};
prog_uchar *line_4p[] PROGMEM={
  line0_4p, line1_4p, line2_4p, line3_4p};

#define CHAR_MIN_4P 32
#define CHAR_MAX_4P 126

SFRGBLEDMatrix::SFRGBLEDMatrix(bool square, byte dispCount, byte pinSS) {
  this->pinSS=pinSS;
  this->square=square;
  this->dispCount=dispCount;
  if(square) {
    width=DISP_LEN*2;
    height=width;
  }else{
    width=DISP_LEN*dispCount;
    height=DISP_LEN;
  }
  pixels=uint16_t(dispCount)*DISPLAY_PIXELS;
  buffSize=DISPLAY_BUFFER_SIZE*dispCount;
  frameBuff=(byte *)calloc((size_t)(buffSize), sizeof(byte)); // FIXME validate if NULL

  setupSPI();
  setupPINs();
};

SFRGBLEDMatrix::~SFRGBLEDMatrix() {
  free(frameBuff);
};

void SFRGBLEDMatrix::setupSPI() {
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV4);
  SPI.setBitOrder(MSBFIRST);
}

void SFRGBLEDMatrix::setupPINs() {
  // FIXME it is hardcoded for Duemilanove
  pinMode(11, OUTPUT); // MOSI
  pinMode(pinSS, OUTPUT); // SS
}

void SFRGBLEDMatrix::show() {
  digitalWrite(pinSS, LOW);
  for(uint16_t p=0;p<buffSize;p++){
    SPI.transfer(*(frameBuff + p));
    delayMicroseconds(64);
  }
  digitalWrite(pinSS, HIGH);
  delayMicroseconds(297);
};

#define X_MAX_4P (byte)(pgm_read_word_near(coffset_4p+c-CHAR_MIN_4P+1)-(byte)pgm_read_word_near(coffset_4p+c-CHAR_MIN_4P))

void SFRGBLEDMatrix::printChar4p(char c, Color color, int x_offset, int y_offset){
  if(c<CHAR_MIN_4P||c>CHAR_MAX_4P)
    return;
  for(int y=0;y<4;y++){
    if(y+y>=height)
      continue;
    uint8_t x_max=X_MAX_4P;
    for(int x=0;x<x_max;x++){
      unsigned int bitOffset;
      byte charData;
      byte pixel;
      if(x+x>=width)
        continue;
      bitOffset=(unsigned int)pgm_read_word_near(coffset_4p+c-CHAR_MIN_4P)+x;
      charData=(byte)pgm_read_word_near((byte *)pgm_read_word_near(&line_4p[y])+bitOffset/8);
      pixel=(charData>>(7-bitOffset%8)) & B00000001;
      if(pixel)
        paintPixel(color, x+x_offset, y+y_offset);
    }
  }
}

void SFRGBLEDMatrix::printString4p(char *s, Color color, int x, int y){
  uint16_t incr;
  for(uint16_t p=0;s[p]!='\0';p++){
    char c;
    c=s[p];
    printChar4p(s[p], color, x, y);
    x+=X_MAX_4P+1;
  }
}

void SFRGBLEDMatrix::paintPixel(Color color, int x, int y) {
  uint16_t startPixel;
  uint16_t startByte;
  // Fix coordinates for SQUARE
  if(square){
    if(y<8){
      x=31-x;
      y=7-y;
    }else
      x-=8;
  }
  // print pixel
  startPixel=(dispCount-1-(x>>3))*DISPLAY_PIXELS + ((7-y)<<3) + (x&7);
  startByte=startPixel*12/8;
  // odd pixels
  if(startPixel&0x01) {
    // XXXX RRRR
    frameBuff[startByte]=(frameBuff[startByte]&0xF0)|(color>>8);
    // GGGG BBBB
    frameBuff[startByte+1]=color&0xFF;
  // even pixels
  }else{
    // RRRR GGGG
    frameBuff[startByte]=color>>4;
    // BBBB XXXX
    frameBuff[startByte+1]=(frameBuff[startByte+1]&0x0F)|(color&0x0F)<<4;
  }
}

Color SFRGBLEDMatrix::getPixel(int x, int y) {
  return 0; // TODO
}

void SFRGBLEDMatrix::fill(Color color){
  // TODO optimize
  for(int x=0;x<width;x++)
    for(int y=0;y<height;y++){
      paintPixel(color, x, y);
  }   
}

void SFRGBLEDMatrix::clear(){
  fill(BLACK);
}
