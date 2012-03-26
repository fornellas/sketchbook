#include <fixFFT.h>
#include <SPI.h>
#include <SFRGBLEDMatrix.h>

#include "pins.h"

#define LOG2_SAMPLES 6
#define SAMPLES_COUNT (1<<LOG2_SAMPLES)

volatile word lost=0;
volatile byte count=0;
volatile int16_t samples[SAMPLES_COUNT];

SFRGBLEDMatrix *display;

ISR(TIMER2_COMPA_vect){
  if(SAMPLES_COUNT==count) {
    lost++;
  }
  else{
    int16_t buff;
    // wait convertion end
    while(!(ADCSRA & (1<<ADIF)));
    // read sample
    buff=ADC;
    samples[count++]=(buff-512);
    // clear bit
    ADCSRA|=(1<<ADIF);
    // start next convertion
    ADCSRA|=(1<<ADSC);
  }
}

void setup(){
  Serial.begin(115200);
  SPI.begin();
  display=new SFRGBLEDMatrix(PIN_MATRIX_SS, 3, 2);
  display->fill(BLACK);
  display->show();

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
  if(SAMPLES_COUNT==count){
    //    Serial.println("==RAW==");
    //    for(byte c=0;c<SAMPLES_COUNT;c++)
    //      Serial.println(samples[c]);

    // FFT
    fft((int16_t *)samples, freq, LOG2_SAMPLES);

    //    Serial.println("==FFT==");
    //    for(byte c=0;c<SAMPLES_COUNT/2;c++){
    //      Serial.print(freq[c]);
    //      Serial.print(' ');
    //    }     
    //    Serial.println("");

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

    // start next convertion
    ADCSRA|=(1<<ADSC);
    // print lost samples
    Serial.print("Lost samples: ");
    Serial.println(lost);
    lost=0;
    // reset counter
    count=0;
    //    delay(10000);
  }
}






