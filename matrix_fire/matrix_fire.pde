#define PIN_SCK  13
#define PIN_MISO 12
#define	PIN_MOSI 11
#define PIN_SS   10

#define PIN_POT  1

#define N_DISP 2


#define ThermistorPIN 0

#define TEMP_MIN 23.0
#define TEMP_MAX 28.0

char frameBuf[64*N_DISP];
byte newFireBuf[(8*N_DISP+2)*(10)];
byte oldFireBuf[(8*N_DISP+2)*(10)];

char scaleHot[]={
  1<<2|2,
  1<<5,
  3<<5,
  6<<5|1<<2,
  7<<5|2<<2,
  7<<5|3<<2,
  7<<5|4<<2,
  7<<5|5<<2,
  7<<5|6<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
  7<<5|7<<2,
};

char scaleCold[]={
  0,
  1,
  2,
  3|1<<2,
  3|2<<2,
  3|3<<2,
  3|4<<2,
  3|5<<2,
  3|6<<2,
  3|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
  2|7<<2,
};

volatile char *scale=scaleHot;

#define SCALE_SIZE 40

#include <math.h>

void swap(){
  unsigned long static lastMillis=0;
  unsigned long currMillis=millis();
  if(currMillis-lastMillis<500)
    return;
  lastMillis=currMillis;
  if(scale==scaleHot)
    scale=scaleCold;
  else
    scale=scaleHot;
}

double Thermistor(int RawADC) {
 long Resistance;  double Temp;
 Resistance=((10240000/RawADC) - 10000);
 Temp = log(Resistance);
 Temp = 1 / (0.001129148 + (0.000234125 * Temp) + (0.0000000876741 * Temp * Temp * Temp));
 Temp = Temp - 273.15;
 return Temp;
}

void printDouble(double val, byte precision) {
  Serial.print (int(val));
  if( precision > 0) {
    Serial.print(".");
    unsigned long frac, mult = 1;
    byte padding = precision -1;
    while(precision--) mult *=10;
    if(val >= 0) frac = (val - int(val)) * mult; else frac = (int(val) - val) * mult;
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
        if(d==0)
          if(c&7<<5&&c&7<<2)
            c=(c&B00011111)|((((c&B11100000)>>5)-1)<<5);
        if('%'==c)
          sendChar('%'-1);
        else
          sendChar(c);
      }
    digitalWrite(PIN_SS, HIGH);
    delayMicroseconds(10);
  }
}

void burn(){
  for(byte y=0;y<8;y++)
    for(byte x=0;x<8*N_DISP;x++)
      frameBuf[8*N_DISP*y+x]=scale[newFireBuf[(8*N_DISP+2)*(y+1)+x+1]];
  sendFrame(frameBuf);
  delay(85);
}

byte f2b(float f){
  byte r;
  r=(byte)f;
  if(f-((float)(int)f)>0.5)
    r++;
  return r;
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
  for(int c=0;c<(8*N_DISP+2)*(10);c++)
    oldFireBuf[c]=0;
    
  attachInterrupt(0, swap, HIGH);
}

void loop(){
  double temp;
  float h;
  float osc;
  int pot;
  float temp_min;
  float temp_max;

  temp=Thermistor(analogRead(ThermistorPIN));
  Serial.print("Current: ");
  printDouble(temp,3);
  Serial.print(" C, Min: ");
  
  pot=analogRead(PIN_POT);

  temp_min=map(pot, 0, 1023, 15, 30);
  temp_max=temp_min+5;

  Serial.print(temp_min);
  Serial.print(" C, Max: ");
  Serial.print(temp_max);
  Serial.println(" C");

  if(temp<temp_min) {
    h=0;
  }else if(temp>temp_max){
    h=1;
  }else{
    h=(temp-temp_min)/(temp_max-temp_min);
  }

  osc=(float)rand()/(float)RAND_MAX;
  if(osc<0.5)
    osc=0.5;

  for(int x=0;x<8*N_DISP+2;x++) {
    byte v;
    if(x<4||x>8*N_DISP+2-4)
      v=0;
    else
      v=(byte)(((float)rand()/(float)RAND_MAX*(float)(SCALE_SIZE-1)*h)*osc);
    oldFireBuf[(8*N_DISP+2)*9+x]=v;
    newFireBuf[(8*N_DISP+2)*9+x]=v;
  }

  for(int y=7;y>=0;y--)
    for(int x=0;x<8*N_DISP;x++) {
      newFireBuf[(8*N_DISP+2)*(y+1)+x+1]=f2b((float)(
//        oldFireBuf[(8*N_DISP+2)*(y)+x]+
//        oldFireBuf[(8*N_DISP+2)*(y)+x+1]+
//        oldFireBuf[(8*N_DISP+2)*(y)+x+2]+
        oldFireBuf[(8*N_DISP+2)*(y+1)+x]+
        oldFireBuf[(8*N_DISP+2)*(y+1)+x+2]+
        oldFireBuf[(8*N_DISP+2)*(y+2)+x]+
        oldFireBuf[(8*N_DISP+2)*(y+2)+x+1]+
        oldFireBuf[(8*N_DISP+2)*(y+2)+x+2]
          )/6.0);
    }
  for(byte c=0;c<(8*N_DISP+2)*(10);c++)
    oldFireBuf[c]=newFireBuf[c];
  burn();
}
