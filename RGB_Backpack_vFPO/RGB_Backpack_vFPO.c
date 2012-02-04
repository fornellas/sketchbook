/* TODO
  - Test data packing with one color per byte, to see if flickering decreases.
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

// receive / display buffers
// Data packing:
//  | RRRR GGGG | BBBB RRRR | GGGG BBBB |
//  |   byte1   |   byte2   |   byte3   |
// Each color has 4bits, and one pixel needs 12bis or 1,5 byte.
volatile uint8_t buffA[DISPLAY_BUFFER_SIZE];
volatile uint8_t buffB[DISPLAY_BUFFER_SIZE];
volatile uint8_t *receiveBuffer=buffA;
volatile uint8_t *displayBuffer=buffB;

// Buffer index counter (0-DISPLAY_BUFFER_SIZE)
volatile uint16_t bufferIndex=0;

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
  SPCR=(1<<SPIE)|(1<<SPE);
}

//
// SPI Interrupt handler
//

ISR(SPI_STC_vect) {
  // Store received byte
  receiveBuffer[bufferIndex++]=SPDR;
  // Zero counter if needed
  if(bufferIndex==DISPLAY_BUFFER_SIZE)
    bufferIndex=0;
  // Send oldest buffer byte to the next board
  SPDR=receiveBuffer[bufferIndex];
}

//
// Send Color Values
//

void sendColorValues(char row_num, uint8_t bitOffset){
  uint8_t ledMax=(row_num<<3)+8;
  for (uint8_t led=row_num<<3;led<ledMax;led++){
    // Get current color
    uint16_t startBit;
    uint8_t value;
    // Find the byte where the pixel color is stored
    startBit=bitOffset+12*led;
    if(startBit%8)
      value=displayBuffer[startBit>>3]&0x0F;
    else {
      value=displayBuffer[startBit>>3]&0xF0;
      value=value>>4;
    }
    //Lower the shift register clock so we can configure the data
    cbi(PORTC, CLK);
    //Compare the current color value to timer_clicks to Pulse Width Modulate
    // the led to create the designated brightness
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

void showDisplayBuffer(void){
  //Increment clicks to determine LED brightness levels, circular 0-15
  timer_clicks = (++timer_clicks)&0x0F;
  
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
    //Sink current through row (Turns colors 'ON' for the given row. Keep in
    // mind that we can only display to 1 row at a time.)
    PORTD=(1<<row_num);
    //Enable the Shift Register Outputs
    cbi(PORTC, EN);
  }
}

//
// Paint a single color of a pixel
//

void paintColor(uint8_t x, uint8_t y, uint16_t colorOffset, uint8_t value){
  uint16_t startBit;
  startBit=colorOffset+((y<<3)+x)*(BITS_PER_COLOR*3);
  if(startBit%8)
    displayBuffer[startBit>>3]=(displayBuffer[startBit>>3]&0xF0)|value;
  else
    displayBuffer[startBit>>3]=(displayBuffer[startBit>>3]&0x0F)|(value<<BITS_PER_COLOR);
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
        
void main(void){
  // Slave select variables
  uint8_t curCS;
  uint8_t oldCS=1;

  // Start I/O and Interrupts
  ioinit();

  // Show start up pattern
  for(int y=0,v=0;y<2;y++)
    for(int x=0;x<8;x++,v++) {
      paint(x,y,v,0,0);
      paint(x,y+2,0,v,0);
      paint(x,y+4,0,0,v);
      paint(x,y+6,v,v,v);
    }
  for(uint16_t c=0;c<3000;c++)
      showDisplayBuffer();  

  // Clear buffers
  for(uint8_t p=0;p<DISPLAY_BUFFER_SIZE;p++)
    displayBuffer[p]=0x00;
  for(uint8_t p=0;p<DISPLAY_BUFFER_SIZE;p++)
    receiveBuffer[p]=0x00;

  //Enable Global Interrupts
  sei();

  //Main loop
  while(1) {
    curCS=PINB & (1 << CS);
    // If CS goes from low to high, SPI transfer is complete
    if (curCS&&!oldCS) {
      cli();
      // swap buffers
      volatile uint8_t *t;
      t=displayBuffer;
      displayBuffer=receiveBuffer;
      receiveBuffer=t;
      // Reset counter
      bufferIndex=0;
      sei();
    }
    oldCS=curCS;
    // Display current buffer
    showDisplayBuffer();
  };
}
