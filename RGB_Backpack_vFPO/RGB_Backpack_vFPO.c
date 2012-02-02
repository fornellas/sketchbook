/* TODO
  - Validate if SPI input is working.
    - Single and more boards
  - Pre-cache color value at interrupt routine to make sendColorValues() faster to decrease flickering.
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
#define DISPLAY_BUFFER_SIZE (DISPLAY_PIXELS*BITS_PER_COLOR*3/8)
#define DISPLAY_WIDTH 8

// Start of this board buffer at SPI data stream
#define START_OF_BUFFER (2+DISPLAY_BUFFER_SIZE*(thisBoardPosition-1))

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

// receive / display buffers
// Data packing:
//  | RRRR GGGG | BBBB RRRR | GGGG BBBB |
//  |   byte1   |   byte2   |   byte3   |
// Each color has 4bits, and one pixel needs 12bis or 1,5 byte.
volatile uint8_t receiveBuffer[DISPLAY_BUFFER_SIZE];
uint8_t displayBuffer[DISPLAY_BUFFER_SIZE];

// Byte counter for each SPI Transfer
volatile uint16_t spiByteCount=0;

// true only after a full frame has been buffered
volatile uint8_t frameReceivedOK=0;

// Number of boards
volatile uint8_t boardCount;

// Position of this board at the array
volatile uint8_t thisBoardPosition;

//Used for PWM to generate different color brightnesses
uint8_t timer_clicks=0;

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
  SPCR = (1 << SPE) | (1 << SPIE);
}

//
// SPI Interrupt handler
//

ISR(SIG_SPI) {
  // byte received
  volatile char value;

  // Halt Interrupts
  cli();

  // Read SPI byte
  value = SPDR;

  // First byte is the position of this board at the board array
  if(0==spiByteCount) {
    thisBoardPosition=value;
    // Decrease the board before passing it to the next board
    SPDR=value-1;
  } else {
    // Second byte is the number of boards
    if(1==spiByteCount)
      boardCount=value;

    // TODO make better valitation of values
    if(boardCount>8)
      boardCount=1;
    if(thisBoardPosition>boardCount)
      boardCount=1;
    
    // Byte 3+ is image data

    // Pass boardCount or image data to the next board 
    SPDR=value;

    // If the byte is for this board, store it at the receive buffer
    if(spiByteCount>=START_OF_BUFFER && spiByteCount<START_OF_BUFFER+DISPLAY_BUFFER_SIZE)
     receiveBuffer[spiByteCount-START_OF_BUFFER]=value;

    // If we finished receiving, set OK to process buffer
    if(START_OF_BUFFER+DISPLAY_BUFFER_SIZE-1==spiByteCount)
      frameReceivedOK=1;

    // If we get more data than we should, invalidate it
    if(START_OF_BUFFER+DISPLAY_BUFFER_SIZE==spiByteCount)
      frameReceivedOK=0;
  }

  // increment byte counter
  spiByteCount++;

  // Enable interrupts
  sei();
}

//
// Send Color Values
//

void sendColorValues(char row_num, uint8_t bitOffset){
  for (uint8_t led=row_num*8;led<(row_num*8)+8;led++){
    // Get current color
    uint16_t startBit;
    uint8_t value;
    // Find the byte where the pixel color is stored
    startBit=bitOffset+12*led;
    if(startBit%8)
      value=displayBuffer[startBit/8]&0x0F;
    else {
      value=displayBuffer[startBit/8]&0xF0;
      value=value>>4;
    }
    //Lower the shift register clock so we can configure the data
    cbi(PORTC, CLK);
    //Compare the current color value to timer_clicks to Pulse Width Modulate the led to create the designated brightness
    if (timer_clicks < value)
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

void showDisplayBuffer(){
  //Increment clicks to determine LED brightness levels, circular 0-15
  timer_clicks = (timer_clicks + 1) & 0x0F;
  
  //Send all 8 rows of colors to the Matrix
  for(char row_num=0;row_num<8;row_num++){
    //Disable the shift registers
    cbi(PORTC, LATCH);

    //Send Red Values
    sendColorValues(row_num, 0);
    //Send Green Values
    sendColorValues(row_num, 8);
    //Send Blue Values
    sendColorValues(row_num, 4);

    //Disable the Shift Register Outputs
    sbi(PORTC, EN);
    //Put the new data onto the outputs of the shift register
    sbi(PORTC, LATCH);
    //Sink current through row (Turns colors 'ON' for the given row. Keep in mind that we can only display to 1 row at a time.)
    PORTD=(1<<row_num);
    //Enable the Shift Register Outputs
    cbi(PORTC, EN);
  }
}

//
// Paint a single color from a pixel
//

void paintColor(uint8_t x, uint8_t y, uint16_t colorOffset, uint8_t value){
  uint16_t startBit;
  startBit=colorOffset+(y*DISPLAY_WIDTH+x)*(BITS_PER_COLOR*3);
  if(startBit%8)
    displayBuffer[startBit/8]=(displayBuffer[startBit/8]&0xF0)|value;
  else
    displayBuffer[startBit/8]=(displayBuffer[startBit/8]&0x0F)|(value<<BITS_PER_COLOR);
}

//
// Paint a single pixel
//

void paint(uint8_t x, uint8_t y, uint8_t r, uint8_t g, uint8_t b){
  paintColor(x,8-y-1,0,r);
  paintColor(x,8-y-1,4,g);
  paintColor(x,8-y-1,8,b);
}

/*****************************************************************************
 * Main
 *****************************************************************************/

int main(void){
  ioinit();

  // Show start up pattern
  for(int y=0,v=0;y<2;y++)
    for(int x=0;x<8;x++,v++) {
      paint(x,y,v,0,0);
      paint(x,y+2,0,v,0);
      paint(x,y+4,0,0,v);
      paint(x,y+6,v,v,v);
    }
  for(uint16_t c=0;c<5000;c++)
      showDisplayBuffer();  

  // Clear display buffer
  for(uint8_t p=0;p<DISPLAY_BUFFER_SIZE;p++)
    displayBuffer[p]=0x00;

  //Enable Global Interrupts
  sei();

  //Main loop
  while(1) {
    //If CS goes high, SPI transfer is complete
    if (PINB & (1 << CS)) {
      // Transfer successfull
      if(frameReceivedOK) {
        // Copy receive buffer to display buffer ()
        cli();
        memcpy((void *)displayBuffer, (void *)receiveBuffer, (size_t)DISPLAY_BUFFER_SIZE);
        sei();
      }
      // Reset counters
      spiByteCount=0;
      frameReceivedOK=0;
    }
    // Display current buffer
    showDisplayBuffer();
  };
  return 0;
}
