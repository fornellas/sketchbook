#include <fixFFT.h>
#include <SPI.h>
#include <SFRGBLEDMatrix.h>

#include "pins.h"

#define LOG2_SAMPLES 6
#define SAMPLES_COUNT (1<<LOG2_SAMPLES)

volatile word lost=0;
volatile byte count=0;
volatile byte pass=0;
volatile boolean readyToProcess=false;
#define LOG2_PASS 6
#define PASS_COUNT (1<<LOG2_SAMPLES)

volatile int16_t samplesA[SAMPLES_COUNT];
volatile int16_t samplesB[SAMPLES_COUNT];

volatile int16_t *samplesWriting;
volatile int16_t *samplesProcessisg;

SFRGBLEDMatrix *display;

ISR(TIMER2_COMPA_vect){
  if(pass==PASS_COUNT){
    if(readyToProcess){
      lost++;
      return;
    }
    pass=0;
    // swap buffers
    volatile int16_t *t;
    t=samplesWriting;
    samplesWriting=samplesProcessisg;
    samplesProcessisg=t;
    readyToProcess=true;
  }

  if(SAMPLES_COUNT==count){
    count=0;
    pass++;
  }

  // wait convertion end
  while(!(ADCSRA & (1<<ADIF)));
  // read sample
  int16_t buff;
  buff=ADC;
  samplesWriting[count++]+=(buff-512);
  // clear bit
  ADCSRA|=(1<<ADIF);
  // start next convertion
  ADCSRA|=(1<<ADSC);
}

void setup(){
  Serial.begin(115200);
  SPI.begin();
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, 3, 2);
  display->fill(BLACK);
  display->show();

  // Prepare buffers
  samplesWriting=samplesA;
  for(byte c=0;c<SAMPLES_COUNT;c++)
    samplesWriting[c]=0;
  samplesProcessisg=samplesB;

  //
  // ADC Setup
  //

  //   - AVCC with external capacitor at AREF pin
  //   - ADC Left Adjust Result
  //   - Analog Channel Selection
  ADMUX=(1<<REFS0)|(0<<ADLAR)|((PIN_LINEIN-14)&0x07);
  //   - ADC Enable
  //   - ADC Start Conversion
  //   - Clear ADIF
  //   - ADC Prescaler Select Bits
  //     - Max 1 0 1 (32)
  //     - Min 0 1 0 (4) < more high freq noise
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADIF)|(1<<ADPS2)|(0<<ADPS1)|(1<<ADPS0);
  // discart first (slow) sample
  while(!(ADCSRA & (1<<ADIF)));
  ADCSRA|=(1<<ADIF);

  //
  // Timer2 @40kHz
  //

  TCCR2A=(1<<WGM21);
  TCCR2B=(1<<CS21);
  OCR2A=50;
  TIFR2=0;
  ASSR=0;
  GTCCR=0;

  //
  // Start ADC capture and Timer2 Interrupt
  //
  ADCSRA|=(1<<ADSC);
  TIMSK2=(1<<OCIE2A);
}

void loop(){
  int16_t freq[SAMPLES_COUNT/2];

  // All samples acquired
  if(readyToProcess){
    // divide
    for(byte c=0;c<SAMPLES_COUNT;c++)
      samplesProcessisg[c]=samplesProcessisg[c]>>LOG2_PASS;

    // FFT
    fft((int16_t *)samplesProcessisg, freq, LOG2_SAMPLES);

    // Draw
    for(byte x=0;x<display->width;x++){
      for(byte y=0;y<display->height;y++){
        if(display->height-y<freq[x])
          display->paintPixel(RGB(0,0,1), x, y);
        else
          display->paintPixel(BLACK, x, y);
      }
    }
    display->show();

    // zero array
    for(byte c=0;c<SAMPLES_COUNT;c++)
      samplesProcessisg[c]=0;

    // print lost samples
    Serial.print("Lost samples: ");
    Serial.println(lost);
    lost=0;
    
    // Release interrupt
    readyToProcess=false;
  }
}













