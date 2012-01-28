#include <Thermistor.h>
#include <avr/pgmspace.h>

#define PIN_SCK  13
#define PIN_MISO 12
#define	PIN_MOSI 11
#define PIN_SS   10

#define N_DISP 2

Thermistor thermistor(0);

char frameBuff[64*N_DISP];

double min_temp;

prog_uint16_t coffset[] PROGMEM={
  0, 1, 2, 5, 10, 13, 17, 21, 22, 24, 26, 31, 34, 36, 38, 39, 43, 46, 49, 52, 55, 58, 61, 64, 67, 70, 73, 74, 76, 78, 80, 82, 85, 88, 91, 94, 97, 100, 103, 106, 109, 112, 115, 118, 122, 125, 130, 134, 138, 141, 145, 148, 151, 154, 158, 162, 167, 170, 173, 177, 179, 183, 185, 188, 191, 193, 196, 199, 202, 205, 208, 210, 212, 215, 216, 219, 222, 224, 229, 232, 235, 238, 241, 243, 246, 249, 252, 255, 260, 263, 266, 269, 272, 273, 276, 280};
prog_uchar line0[] PROGMEM={
  0x6a, 0x9c, 0xb5, 0xaa, 0x0, 0x29, 0x6c, 0x7b, 0xff, 0x90, 0x33, 0x59, 0xef, 0xdd, 0xe6, 0x64, 0x65, 0xb3, 0x67, 0xe6, 0x63, 0x6f, 0xf1, 0xa1, 0x8, 0x8, 0x79, 0x12, 0x0, 0x19, 0x1, 0x0, 0x1, 0x7b, 0xe0};
prog_uchar line1[] PROGMEM={
  0x6f, 0xf1, 0x26, 0x5c, 0x80, 0x57, 0x17, 0x64, 0x3e, 0xc7, 0x8d, 0xbe, 0x5d, 0x25, 0x46, 0xa6, 0xf6, 0x6c, 0xd8, 0xa6, 0x6a, 0xa9, 0x48, 0xd0, 0xbc, 0xdb, 0xbc, 0x36, 0xd6, 0x56, 0xef, 0xdb, 0x1b, 0xce, 0xb5};
prog_uchar line2[] PROGMEM={
  0x7, 0xda, 0x4a, 0x5d, 0xdc, 0x95, 0x43, 0xcf, 0x57, 0x98, 0x44, 0xf6, 0x59, 0xaf, 0x57, 0xa5, 0x6e, 0x75, 0xe2, 0xa5, 0x77, 0x52, 0x44, 0x80, 0x5b, 0x2e, 0xdb, 0xba, 0xad, 0xbb, 0xc9, 0x5b, 0x54, 0x52, 0xaa};
prog_uchar line3[] PROGMEM={
  0x42, 0xb4, 0xb1, 0xaa, 0xa3, 0xb, 0xfc, 0x77, 0x9c, 0xe7, 0x93, 0xb9, 0xef, 0x1d, 0xea, 0x7c, 0x65, 0xa3, 0xdc, 0x98, 0xa3, 0x57, 0xe3, 0x8e, 0x3c, 0xdb, 0xbb, 0x57, 0xad, 0x51, 0xd9, 0x34, 0xab, 0xbb, 0xe0};
prog_uchar *line[] PROGMEM={
  line0, line1, line2, line3};

#define CHAR_MIN 32
#define CHAR_MAX 126

void printDouble(double val, byte precision) {
  Serial.print (int(val));
  if( precision > 0) {
    Serial.print(".");
    unsigned long frac, mult = 1;
    byte padding = precision -1;
    while(precision--) mult *=10;
    if(val >= 0) frac = (val - int(val)) * mult; 
    else frac = (int(val) - val) * mult;
    unsigned long frac1 = frac;
    while(frac1 /= 10) padding--;
    while(padding--) Serial.print("0");
    Serial.print(frac,DEC) ;
  }
}

void sendChar(char cData){
  SPDR = cData;
  while(!(SPSR&(1<<SPIF)));
}

void sendFrame(char *frame) {
  byte d, x, y;
  char c;
  for(d=0;d<N_DISP;d++){
    digitalWrite(PIN_SS, LOW);
    delayMicroseconds(500);
    for(x=0;x<8;x++)
      for(y=0;y<8;y++){
        c=*(frame + 8*N_DISP*x + (N_DISP-(d+1))*8 + (7-y) );
        if(0==d)
          if( c&(7<<2) ) {
            if( ((c&(7<<5))>>5)>2 )
              c=(c&B00011111)|((((c&(7<<5))>>5)-2)<<5);
            else
              if( ((c&(7<<5))>>5)>1 )
                c=(c&B00011111)|((((c&(7<<5))>>5)-1)<<5);
            if( (c&3)>1 )
              c=(c&B11111100)|((c&3)-1);
          }
        if('%'==c)
          sendChar('%'-1);
        else
          sendChar(c);
      }
    digitalWrite(PIN_SS, HIGH);
    delayMicroseconds(10);
  }
}

void printChar(char c, byte color, byte x_pos, byte y_pos) {
  for(byte y=0;y<4;y++){
    byte x_max=(byte)pgm_read_word_near(coffset+c-CHAR_MIN+1)-(byte)pgm_read_word_near(coffset+c-CHAR_MIN);

    if(y+y_pos>=8)
      continue;

    for(byte x=0;x<x_max;x++){
      byte f;
      unsigned int bitOffset;
      byte charData;
      byte pixel;

      if(x+x_pos>=8*N_DISP)
        continue;

      bitOffset=(unsigned int)pgm_read_word_near(coffset+c-CHAR_MIN)+x;
      charData=(byte)pgm_read_word_near((byte *)pgm_read_word_near(&line[y])+bitOffset/8);
      pixel=(charData>>(7-bitOffset%8)) & B00000001;
      if(pixel)
        frameBuff[(y+y_pos)*8*N_DISP+x+x_pos]=color;
    }
  }
}

void setup(){
  SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR1)|(1<<SPR0);
  SPSR = SPSR & B11111110;

  pinMode(PIN_SCK, OUTPUT);
  digitalWrite(PIN_SCK, LOW);
  pinMode(PIN_MOSI, OUTPUT);
  pinMode(PIN_SS, OUTPUT);
  digitalWrite(PIN_SS, HIGH);
  delayMicroseconds(500);

  digitalWrite(PIN_SS, LOW);
  delayMicroseconds(500);
  sendChar('%');
  sendChar(N_DISP);
  digitalWrite(PIN_SS, HIGH);
  delayMicroseconds(10);
  Serial.begin(115200);

  min_temp=200;

  for(byte i=0;i<64*N_DISP;i++)
    frameBuff[i]=0;
  sendFrame(frameBuff);

}

void loop(){
  double temp;

  temp=thermistor.read();
  Serial.print("Temperature: ");
  printDouble(temp,3);
  Serial.println("");

  if(temp<min_temp)
    min_temp=temp;

  for(byte x=0;x<8*N_DISP;x++)
    for(byte y=0;y<8;y++)
      if(y<4)
        frameBuff[y*8*N_DISP+x]=1;
      else
        frameBuff[y*8*N_DISP+x]=1<<2;

  printChar((char)(48+((int)temp)/10), 7<<5, 0, 0);
  printChar((char)(48+((int)temp)%10), 7<<5, 4, 0);
  printChar('.', 7<<5, 8, 0);
  printChar((char)(48+((int)(temp*10)-10*(int)temp)), 7<<5, 10, 0);
  printChar('C', 7<<5, 14, 0);

  printChar((char)(48+((int)min_temp)/10), 7<<5|7<<2, 0, 4);
  printChar((char)(48+((int)min_temp)%10), 7<<5|7<<2, 4, 4);
  printChar('.', 7<<5|7<<2, 8, 4);
  printChar((char)(48+((int)(min_temp*10)-10*(int)min_temp)), 7<<5|7<<2, 10, 4);
  printChar('C', 7<<5|7<<2, 14, 4);

  sendFrame(frameBuff);
  delay(333);

}














