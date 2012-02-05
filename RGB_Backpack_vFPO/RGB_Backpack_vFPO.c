/* TODO
  - Try do optmize main loop to allow 5bits per color without flickering.
*/

/*****************************************************************************
 * Includes
 *****************************************************************************/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

/*****************************************************************************
 * Defines
 *****************************************************************************/

#define DISPLAY_PIXELS 64
#define BITS_PER_COLOR 4
#define DISPLAY_BUFFER_SIZE (DISPLAY_PIXELS*BITS_PER_COLOR*3>>3)

// Port B
#define CS 2
#define MOSI 3
#define MISO 4
#define SCK 5

// Port C
#define CLK 0
#define DATA 1
#define LATCH 2
#define CLR 3
#define EN 4

/*****************************************************************************
 * Macros
 *****************************************************************************/

// used at shift_out_line()
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

/*****************************************************************************
 * Global variables
 *****************************************************************************/

// Data packing:
//  | RRRR GGGG | BBBB RRRR | GGGG BBBB |
//  |   byte1   |   byte2   |   byte3   |

// Swappable buffers
volatile uint8_t redBufferA[DISPLAY_PIXELS];
volatile uint8_t redBufferB[DISPLAY_PIXELS];
volatile uint8_t greenBufferA[DISPLAY_PIXELS];
volatile uint8_t greenBufferB[DISPLAY_PIXELS];
volatile uint8_t blueBufferA[DISPLAY_PIXELS];
volatile uint8_t blueBufferB[DISPLAY_PIXELS];

// Receive buffers
volatile uint8_t *receiveBufferRed=redBufferA;
volatile uint8_t *receiveBufferGreen=greenBufferA;
volatile uint8_t *receiveBufferBlue=blueBufferA;
volatile uint8_t receiveBufferRaw[DISPLAY_BUFFER_SIZE];
volatile uint8_t colorBufferIndex=0;
volatile uint8_t receiveBufferRawIndex=0;
volatile uint8_t twoPixels=0;

// Display buffers
volatile uint8_t *displayBufferRed=redBufferB;
volatile uint8_t *displayBufferGreen=greenBufferB;
volatile uint8_t *displayBufferBlue=blueBufferB;

//Used for PWM to generate different color brightnesses
uint8_t timerClicks=0;

/*****************************************************************************
 * Functions
 *****************************************************************************/

//
// Initialize I/O and interrupt
//

void ioinit(void) {
  //Enable internal pull-up resistor on MISO
  DDRB = (1 << MISO);		
  //Set SPI Outputs
  PORTB = (1 << MOSI) | (1 << CS) | (1 << SCK);

  //Set outputs to the shift registers
  DDRC = 0x1F;
  //All Port D pins are outputs to the Sync Driver
  DDRD = 0xFF;

  //Set initial pin states
  sbi(PORTC, CLK);
  sbi(PORTC, CLR);
  sbi(PORTC, DATA);
  sbi(PORTC, LATCH);
  sbi(PORTC, EN);

  //Enable SPI / SPI Interrupts
  SPCR=(1<<SPIE)|(1<<SPE);
}

//
// SPI Interrupt handler
//

ISR(SPI_STC_vect) {
  // Save raw input data
  receiveBufferRaw[receiveBufferRawIndex]=SPDR;

  // Process data every 3 bytes (2 pixels)
  if(twoPixels==2){
    receiveBufferRed[colorBufferIndex]=(receiveBufferRaw[receiveBufferRawIndex-2]&0xF0)>>4;
    receiveBufferGreen[colorBufferIndex]=receiveBufferRaw[receiveBufferRawIndex-2]&0x0F;
    receiveBufferBlue[colorBufferIndex]=(receiveBufferRaw[receiveBufferRawIndex-1]&0xF0)>>4;
    colorBufferIndex++;
    receiveBufferRed[colorBufferIndex]=receiveBufferRaw[receiveBufferRawIndex-1]&0x0F;
    receiveBufferGreen[colorBufferIndex]=(receiveBufferRaw[receiveBufferRawIndex]&0xF0)>>4;
    receiveBufferBlue[colorBufferIndex]=receiveBufferRaw[receiveBufferRawIndex]&0x0F;
    colorBufferIndex++;
  }

  // Increment indexes
  if(colorBufferIndex==DISPLAY_PIXELS)
    colorBufferIndex=0;
  receiveBufferRawIndex++;
  if(receiveBufferRawIndex==DISPLAY_BUFFER_SIZE)
    receiveBufferRawIndex=0;
  twoPixels++;
  if(twoPixels==3)
    twoPixels=0;

  // Send old data to the next board
  SPDR=receiveBufferRaw[receiveBufferRawIndex];
}

//
// Send Color Values
//

void sendColorValues(char row, volatile uint8_t *colorValue){
  uint8_t ledMax=(row<<3)+8;
  for (uint8_t led=row<<3;led<ledMax;led++){
    //Lower the shift register clock so we can configure the data
    cbi(PORTC, CLK);
    //Compare the current color value to timerClicks to Pulse Width Modulate
    // the led to create the designated brightness
    if (timerClicks < colorValue[led])
      sbi(PORTC, DATA);
    else
      cbi(PORTC, DATA);
   //Raise the shift register clock to lock in the data
    sbi(PORTC, CLK);
  }
}

//
// Show display buffer
//

void showDisplayBuffer(void){
  //Increment clicks to determine LED brightness levels, circular 0-15
  timerClicks = (++timerClicks)&0xF;
  
  //Send all 8 rows of colors to the Matrix
  for(char row=0;row<8;row++){
    //Disable the shift registers
    cbi(PORTC, LATCH);

    //Send Red Values
    sendColorValues(row, displayBufferRed);
    //Send Blue Values
    sendColorValues(row, displayBufferBlue);
    //Send Green Values
    sendColorValues(row, displayBufferGreen);

    //Disable the Shift Register Outputs
    sbi(PORTC, EN);
    //Put the new data onto the outputs of the shift register
    sbi(PORTC, LATCH);
    //Sink current through row (Turns colors 'ON' for the given row. Keep in
    // mind that we can only display to 1 row at a time.)
    PORTD=(1<<row);
    //Enable the Shift Register Outputs
    cbi(PORTC, EN);
  }
}

/*****************************************************************************
 * Main
 *****************************************************************************/
        
void main(void){
  // Slave select variables
  uint8_t curCS;
  uint8_t oldCS=1;

  // Start I/O and Interrupts
  ioinit();

  // Show start up pattern
  for(int p=0;p<16;p++){
    displayBufferRed[p]=p;
    displayBufferGreen[p]=0;
    displayBufferBlue[p]=0;
  }
  for(int p=16;p<32;p++){
    displayBufferRed[p]=0;
    displayBufferGreen[p]=p-16;
    displayBufferBlue[p]=0;
  }
  for(int p=32;p<48;p++){
    displayBufferRed[p]=0;
    displayBufferGreen[p]=0;
    displayBufferBlue[p]=p-32;
  }
  for(int p=48;p<64;p++){
    displayBufferRed[p]=p-48;
    displayBufferGreen[p]=p-48;
    displayBufferBlue[p]=p-48;
  }
  for(uint16_t c=0;c<5000;c++)
      showDisplayBuffer();  

  // Clear buffers
  for(uint8_t p=0;p<DISPLAY_PIXELS;p++){
    displayBufferRed[p]=0x00;
    displayBufferGreen[p]=0x00;
    displayBufferBlue[p]=0x00;
  }
  for(uint8_t p=0;p<DISPLAY_PIXELS;p++){
    receiveBufferRed[p]=0x00;
    receiveBufferGreen[p]=0x00;
    receiveBufferBlue[p]=0x00;
  }

  //Enable Interrupts
  sei();

  //Main loop
  while(1) {
    curCS=PINB & (1 << CS);
    // If CS goes from low to high, SPI transfer is complete
    if (curCS&&!oldCS) {
      cli();
      // swap buffers
      volatile uint8_t *t;
      t=displayBufferRed;
      displayBufferRed=receiveBufferRed;
      receiveBufferRed=t;
      t=displayBufferGreen;
      displayBufferGreen=receiveBufferGreen;
      receiveBufferGreen=t;
      t=displayBufferBlue;
      displayBufferBlue=receiveBufferBlue;
      receiveBufferBlue=t;
      // Reset counters
      colorBufferIndex=0;
      receiveBufferRawIndex=0;
      twoPixels=0;
      sei();
    }
    oldCS=curCS;
    // Display current buffer
    showDisplayBuffer();
  };
}
