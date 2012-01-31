/*********************************************************
						Global MACROS
*********************************************************/
#define sbi(var, mask)   ((var) |= (uint8_t)(1 << mask))
#define cbi(var, mask)   ((var) &= (uint8_t)~(1 << mask))

/*********************************************************
						I/O Definitions
*********************************************************/
//Port C Definitions
#define CLK		0
#define DATA	1
#define LATCH	2
#define CLR		3
#define EN		4

//Port B Definitions
#define CS		2
#define MOSI	3
#define MISO	4
#define SCK		5

/*********************************************************
					  General Definitions
*********************************************************/
#define RUN_COUNT_ADDRESS	0
#define NUM_BOARDS_ADDRESS	1	//EEPROM location of "NUM_BOARDS" parameters
#define NUM_LEDS	64	//Defines how many LEDs are on each matix (Shouldn't be changed)

/*********************************************************
				Function Prototypes
*********************************************************/
/*
Usage:		ioinit();
Purpose:	Initialize I/O, Setup SPI Hardware
Parameters:	None
Return:		None
*/
void ioinit (void);
/*
Usage:		splash_screen();
Purpose:	Tests all of the LEDs. Function will cause matrix to go all Red for 300ms, then all Green for 300ms, then all Blue for 300ms, then display a "smiley"
Parameters:	None
Return:		None
*/
void splash_screen(void);
/*
Usage:		parse_frame();
Purpose:	Parses the red, green and blue values out of the temporary frame buffer and into separate color buffers
Parameters:	char * postBuffer - 64 position array that contains the rgb values
			of each LED for the Matrix starting with LED 0.
Return:		None.
*/
void parse_frame(char * postBuffer);
/*
Usage:		post_frames();
Purpose:	Puts the current image located in the red_frame, green_frame and blue_frame buffers onto the RGB matrix
Parameters:	None
Return:		None
*/
void post_frames(void);
/*
Usage:		shift_out_line(1);
Purpose:	Sends a single row of colors to the RGB matrix.
Parameters:	input row_num - Designates which row will be sent to the matrix
Return:		None
*/
void shift_out_line(volatile uint8_t row_num);
/*
Usage:		delay_us(250);
Purpose:	Delays the firmware for ~1us
Parameters:	input x - number of microseconds to delay
Return:		None
*/
void delay_us(uint8_t x);
/*
Usage:		delay_ms(10);
Purpose:	Delays the firmware for ~1ms
Parameters:	input x - number of ms to delay
Return:		None
*/
void delay_ms(uint16_t x);
/*
Description: Reads the EEPROM data at "Address" and returns the character
Pre: Unsigned Int Address is the address to be read
Post: Character at "Address" is returned
Usage: 	unsigned char Data;
		Data=read_from_EEPROM(0);
*/
unsigned char read_from_EEPROM(unsigned int Address);
/*
Description: Writes an unsigned char(Data)  to the EEPROM at the given Address
Pre: Unsigned Int Address contains address to be written to
	 Unsigned Char Data contains data to be written
Post: EEPROM "Address" contains the "Data"
Usage: write_to_EEPROM(0, 'A');
*/
void write_to_EEPROM(unsigned int Address, unsigned char Data);

/*
Usage:		fillBoardCount(NUM_BOARDS);
Purpose:	Fills the display buffer with the number of boards programmed into the Matrix
Inputs: 	char number - The number to display on the LED matrix
Return:		None
*/
void fillBoardCount(char number);