#include <fixFFT.h>

#define PIN_LINEIN_L A1
#define PIN_LINEIN_R A2

#define LOG2_SAMPLES 6
#define SAMPLES_COUNT (1<<LOG2_SAMPLES)

volatile word lost=0;
volatile byte count=0;
volatile int16_t samples[SAMPLES_COUNT];

ISR(TIMER1_COMPA_vect){
  if(SAMPLES_COUNT==count) {
    lost++;
  }
  else{
    int16_t b;
    // wait convertion end
    while(!(ADCSRA & (1<<ADIF)));
    // read sample
    //    samples[count++]=ADCH;
    b=ADCH;
    samples[count++]=b-128;
    // clear bit
    ADCSRA|=(1<<ADIF);
    // start next convertion
    ADCSRA|=(1<<ADSC);
  }
}

void
setup(){
  Serial.begin(115200);  
  // 16-bit Timer/Counter1
  // 20kHz sampling (must be double of max mic freq)

  TCCR1A=0;
  //   - Clear Timer on Compare Match (CTC) Mode
  //   - 16MHz / 8x prescaling = 2MHz
  TCCR1B=(1<<WGM12)|(0<<CS12)|(1<<CS11)|(0<<CS10);
  TCCR1C=0;
  //   - Compare
  // 100=22kHz
  // 45 = 44444.444kHz
  OCR1A=100;
  //   - Enable compare interrupt
  TIMSK1=(1<<OCIE1A);
  TIFR1=0;

  // Analog-to-Digital Converter

  //   - AVCC with external capacitor at AREF pin
  //   - ADC Left Adjust Result
  //   - Analog Channel Selection
  ADMUX=(1<<REFS0)|(1<<ADLAR)|((PIN_LINEIN_L-14)&0x07);
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
  ADCSRA|=(1<<ADSC);

  // enable interrupts
  sei();

  Serial.println("==Boot complete==");
}

void
loop(){
  int16_t freq[SAMPLES_COUNT/2];
  // convertion finished
  if(SAMPLES_COUNT==count){
    // process
    Serial.println("==RAW==");
    for(byte c=0;c<SAMPLES_COUNT;c++)
      Serial.println(samples[c]);

    fft((int16_t *)samples, freq, LOG2_SAMPLES);

    Serial.println("==FFT==");
    for(byte c=0;c<SAMPLES_COUNT/2;c++)
      Serial.println(freq[c]);

    // start next convertion
    ADCSRA|=(1<<ADSC);
    // print lost samples
    Serial.print("Lost samples: ");
    Serial.println(lost);
    lost=0;
    // reset counter
    count=0;
    delay(10000);
  }
}












