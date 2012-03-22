#define PIN_LINEIN_L A1
#define PIN_LINEIN_R A2

#define SAMPLES_COUNT 255

volatile boolean finish=0;
volatile byte count=0;
volatile byte samples[SAMPLES_COUNT];

ISR(TIMER1_COMPA_vect){
  if(SAMPLES_COUNT==count) {
    finish=1;
  }
  else{
    // wait convertion end
    while(!(ADCSRA & (1<<ADIF)));
    // read sample
    samples[count++]=ADCH;
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
  //   - Compare = 100
  OCR1A=100;
  //   - Enable compare interrupt
  TIMSK1=(1<<OCIE1A);
  TIFR1=0;

  // Analog-to-Digital Converter

  //   - AVCC with external capacitor at AREF pin
  //   - ADC Left Adjust Result
  //   - Analog Channel Selection
  ADMUX=(1<<REFS0)|(1<<ADLAR)|((PIN_LINEIN_R-14)&0x07);
  //   - ADC Enable
  //   - ADC Start Conversion
  //   - Clear ADIF
  //   - ADC Prescaler Select Bits
  //     - Max 1 0 1 (32)
  //     - Min 0 1 0 (4) < more high freq noise
  ADCSRA=(1<<ADEN)|(1<<ADSC)|(1<<ADIF)|(0<<ADPS2)|(1<<ADPS1)|(0<<ADPS0);


  // enable interrupts
  sei();
}

void
loop(){
  // convertion finished
  if(SAMPLES_COUNT==count){
    Serial.println("=============");
    for(byte c=0;c<SAMPLES_COUNT;c++)
      Serial.println(samples[c]);
    ADCSRA|=(1<<ADSC);
    count=0;
    delay(10000);
  }else
    Serial.println("Waiting");
}





