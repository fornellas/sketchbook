/*
    9/13/10
    Copyright Spark Fun Electronics© 2010
    By Ryan Owens
    
    ATmega328p
    
    Uses RGB matrix backpack v2 (v24.brd)
	
	Program accepts values from SPI and displays them on the attached LED matrix. 
	Board can be configured for a multi-board configuration over SPI. 
	Board accepts NUM_BOARDS*NUM_LEDS bytes and then displays the LED values according
	to which position in the system the board is located.

*/

#include <inttypes.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "RGB_Backpack_v5.h"

/*********************************************************
					  Global Variables
*********************************************************/
volatile uint8_t red_frame[64];		//Each byte in the buffer represents the brightness of the corresponding Red LED on the matrix
volatile uint8_t green_frame[64];	//Each byte in the buffer represents the brightness of the corresponding GREEN LED on the matrix
volatile uint8_t blue_frame[64];	//Each byte in the buffer represents the brightness of the corresponding BLUE LED on the matrix
volatile char myBuffer[512];		//Temporary Buffer that holds the incoming data from the SPI Bus. This data is parsed into the Red,Green and Blue Frame Buffers
volatile unsigned int frame_index = 0;	//Keeps track of the current index
volatile uint8_t new_frame;			//Flag telling the main firmware that enough data has been received and a new image can be displayed onto the matrix

volatile uint8_t timer_clicks=0;	//Used for PWM to generate different color brightnesses

volatile uint8_t NUM_BOARDS=0, RUN_COUNT=0;	
volatile char command_mode=0;
volatile char value=0;

const char numbers[] = {
0x00,0x00,0x84,0x82,0xFF,0x80,0x80,0x00, //49/ --> 1
0x00,0x00,0x82,0xC1,0xA1,0x91,0x8E,0x00, //50/ --> 2
0x00,0x00,0x42,0x81,0x89,0x76,0x00,0x00, //51/ --> 3
0x00,0x0F,0x08,0x08,0x08,0xFF,0x00,0x00, //52/ --> 4
0x00,0x00,0x00,0x8F,0x89,0x89,0x71,0x00, //53/ --> 5
0x00,0x00,0x7C,0x8A,0x89,0x89,0x71,0x00, //54/ --> 6
0x00,0x01,0xC1,0x31,0x09,0x05,0x03,0x00, //55/ --> 7
0x00,0x76,0x89,0x89,0x89,0x89,0x76,0x00, //56/ --> 8
};

/*********************************************************
			Interrupt Service Routines
*********************************************************/
/*
Usage:		Automatic as long as Global Interrupts are enabled (sei())
Purpose:	Initialize I/O, Setup SPI Hardware
Parameters:	None
Return:		None
*/
ISR (SIG_SPI) 
{
	cli();	//Halt Interrupts
	value=SPDR;	//Get the data from the SPI bus and put it into the temporary frame buffer
	//If we didn't receive a special character add the value to the display buffer
	if(!command_mode && value != '%' && value != 0x26)
	{
		myBuffer[frame_index++] = value;	//Add value to buffer and increment the index.
		SPDR = value;	//Send the value to the next board in the system.
	}
	//If the board is in command mode we need to set the NUM_BOARDS parameter
	if(command_mode){
		if(value > 0 && value < 9){	//Make sure we get a number between 0 and 8
			NUM_BOARDS=value;	//Set the NUM_BOARDS parameter for this matrix.	
			SPDR = (value-1);	//Send the NUM_BOARDS parameter to the next board in the system
			write_to_EEPROM(NUM_BOARDS_ADDRESS, NUM_BOARDS);	//Save the NUM_BOARDS to EEPROM
		}
		command_mode=0;	//Reset the command_mode flag
	}
	//If we get the '%' character we should enter command mode.
	if(value == '%'){
		command_mode=1;
		SPDR = '%';	//Send command mode to the next board too.
	}
	//0x26 is a special character to reset the frame index.
	if(value == 0x26){
		frame_index=0;
		SPDR = 0x26;
	}
	sei();	//Re-enable interrupts
}

/*********************************************************
						Main Code
*********************************************************/
int main (void)
{
	ioinit ();	//Initialize the I/O and the SPI port
	
	//Make sure all the pixels are working
	if(RUN_COUNT < 2)splash_screen();
	
	
	fillBoardCount(NUM_BOARDS);	//Put the board number into the display frame
	for(int i=0; i<5000; i++)
	{
		post_frames();
		timer_clicks = (timer_clicks + 1) & 0x07; //0b00000111; //Circular 0 to 7
	}
	
	sei();	//Enable Global Interrupts
	while(1)
	{
        if (PINB & (1<<CS)) //If CS goes high, SPI com is complete
		{
			new_frame=1;
		}
		//New frame is set when enought bytes have been received for the whole system
		if(new_frame==1){
			new_frame=0;	//Reset the frame flag.
			parse_frame(&myBuffer[(NUM_BOARDS-1)*64]);	//Parse the buffer at the location
				//for this specific board.
		}

		post_frames();	//Put the colors onto the matrix.
		//Increment clicks to determine LED brightness levels
		timer_clicks = (timer_clicks + 1) & 0x07; //0b00000111; //Circular 0 to 7
	}
	
	return (0);
}

/*********************************************************
						Functions
*********************************************************/
void ioinit (void)
{
	//1 = Output, 0 = Input
	DDRB = (1<<MISO);	//Set MISO as an output, all other SPI as inputs
	PORTB = (1<<MOSI)|(1<<CS)|(1<<SCK);	//Enable pull-ups on SPI Lines
	
	DDRC = 0x1F;	//Set outputs to the shift registers
	DDRD = 0xFF;	//All Port D pins are outputs to the Sync Driver

	//Set initial pin states
	sbi(PORTC, CLK);
	sbi(PORTC, CLR);
	sbi(PORTC, DATA);
	sbi(PORTC, LATCH);
	sbi(PORTC, EN); 
	
	//Setup the SPI Hardware
	SPCR = (1<<SPE) | (1<<SPIE)|(1<<SPR1)|(1<<SPR0); //Enable SPI, Enable SPI Interrupts
	
	//Load the NUM_BOARDS parameter from EEPROM
	RUN_COUNT = read_from_EEPROM(RUN_COUNT_ADDRESS);
	if(RUN_COUNT == 0xFF){
		RUN_COUNT = 1;
		write_to_EEPROM(RUN_COUNT_ADDRESS, RUN_COUNT);
		NUM_BOARDS=1;
		write_to_EEPROM(NUM_BOARDS_ADDRESS, NUM_BOARDS);
	}
	else{
		RUN_COUNT = read_from_EEPROM(RUN_COUNT_ADDRESS);
		if(RUN_COUNT < 10){
			RUN_COUNT+=1;
			write_to_EEPROM(RUN_COUNT_ADDRESS, RUN_COUNT);
		}
		NUM_BOARDS=read_from_EEPROM(NUM_BOARDS_ADDRESS);
		if(NUM_BOARDS > 8)NUM_BOARDS=1;
	}
		
}

void splash_screen(void)
{
	uint16_t i;
	cli();
	//Fill red
	for(i = 0 ; i < 64 ; i++)
	{
		red_frame[i] = 7;
		green_frame[i] = 0;
		blue_frame[i] = 0;
	}

	for(i = 0 ; i < 1000 ; i++)
		post_frames();

	//Fill green
	for(i = 0 ; i < 64 ; i++)
	{
		red_frame[i] = 0;
		green_frame[i] = 7;
		blue_frame[i] = 0;
	}

	for(i = 0 ; i < 1000 ; i++)
		post_frames();

	//Fill blue
	for(i = 0 ; i < 64 ; i++)
	{
		red_frame[i] = 0;
		green_frame[i] = 0;
		blue_frame[i] = 7;
	}

	for(i = 0 ; i < 1000 ; i++)
		 post_frames(); 		 

	PORTD = 0; //Turn off display
}

void parse_frame(char * postBuffer)
{
    uint8_t color_value;
    
    for(int LED = 0 ; LED < 64 ; LED++)
    {
        color_value = postBuffer[63-LED];
        red_frame[LED] = (color_value & 0xE0) >> 5;	//(temp & 0b11100000) >> 5; Highes 3 bits represent the Red value for the current LED
        green_frame[LED] = (color_value & 0x1C) >> 2; 	//(temp & 0b00011100) >> 2; Next 3 bits represent the Green value for the current LED
        blue_frame[LED] = (color_value & 0x03); 		//(temp & 0b00000011); Final 2 bits represent the Blue value for the current LED
    }
    new_frame = 0; //Reset new frame flag
}

void post_frames(void)
{	
	for(char row = 0 ; row < 8; row++)shift_out_line(row);	//Send all 8 rows of colors to the Matrix
}

void shift_out_line(volatile uint8_t row_num)
{
	cbi(PORTC, LATCH);	//Disable the shift registers

	//Send Red Values
	for(uint8_t LED = row_num*8 ; LED < (row_num*8)+8 ; LED++) //Step through bits
	{
		cbi(PORTC, CLK);	//Lower the shift register clock so we can configure the data
		
		//Compare the current color value to timer_clicks to Pulse Width Modulate the LED to create the designated brightness
		if(timer_clicks < red_frame[LED])
			sbi(PORTC, DATA);
		else
			cbi(PORTC, DATA);

		sbi(PORTC, CLK);	//Raise the shift register clock to lock in the data
	}
	//Send Blue Values
	for(uint8_t LED = row_num*8 ; LED < (row_num*8)+8 ; LED++) //Step through bits
	{
		cbi(PORTC, CLK);	//Lower the shift register clock so we can configure the data
	
		//Compare the current color value to timer_clicks to Pulse Width Modulate the LED to create the designated brightness
		if(timer_clicks < blue_frame[LED])
			sbi(PORTC, DATA);
		else
			cbi(PORTC, DATA);

		sbi(PORTC, CLK);	//Raise the shift register clock to lock in the data
	}
	//Send Green Values
	for(uint8_t i = row_num*8 ; i < (row_num*8)+8 ; i++) //Step through bits
	{
		cbi(PORTC, CLK);	//Lower the shift register clock so we can configure the data
	
		//Compare the current color value to timer_clicks to Pulse Width Modulate the LED to create the designated brightness
		if(timer_clicks < green_frame[i])
			sbi(PORTC, DATA);
		else
			cbi(PORTC, DATA);

		sbi(PORTC, CLK);	//Raise the shift register clock to lock in the data
	}

	sbi(PORTC, EN);		//Disable the Shift Register Outputs
	sbi(PORTC, LATCH);	//Put the new data onto the outputs of the shift register

	PORTD = (1<<row_num); //Sink current through row (Turns colors 'ON' for the given row. Keep in mind that we can only display to 1 row at a time.)

	cbi(PORTC, EN);		//Enable the Shift Register Outputs
}

void delay_us(uint8_t x)
{
	TIFR0 = (1<<TOV0); //Clear any interrupt flags on Timer2
	TCNT0 = 256 - x; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
	while( (TIFR0 & (1<<TOV0)) == 0);

	//Double the delay because we are using 16MHz and 8 prescalar

	TIFR0 = (1<<TOV0); //Clear any interrupt flags on Timer2
	TCNT0 = 256 - x; //256 - 125 = 131 : Preload timer 2 for x clicks. Should be 1us per click
	while( (TIFR0 & (1<<TOV0)) == 0);
}

void delay_ms(uint16_t x)
{
	for ( ; x > 0 ; x--)
	{
		delay_us(250);
		delay_us(250);
		delay_us(250);
		delay_us(250);
	}
}

void write_to_EEPROM(unsigned int Address, unsigned char Data)
{
    //Interrupts are globally disabled!
	
	while(EECR & (1<<EEPE)); //Wait for last Write to complete
	//May need to wait for Flash to complete also!
	EEAR = Address;			//Assign the Address Register with "Address"
	EEDR=Data;				//Put "Data" in the Data Register
	EECR |= (1<<EEMPE); 	//Write to Master Write Enable
	EECR |= (1<<EEPE);  	//Start Write by setting EE Write Enable
	
}

unsigned char read_from_EEPROM(unsigned int Address)
{
	//Interrupts are globally disabled!
	
	while(EECR & (1<<EEPE));	//Wait for last Write to complete
	EEAR = Address;				//Assign the Address Register with "Address"
	EECR |= (1<<EERE); 			//Start Read by writing to EER
	return EEDR;				//EEPROM Data is returned
}

void fillBoardCount(char number)
{
	number-=1;
	for(int column=7; column>=0; column--)
    {
        for(int row=0; row<8; row++)
        {
            if(numbers[number*8+column] & (1<<row))myBuffer[row*8+(7-column)]=0xE0;
            else myBuffer[row*8+(7-column)]=0x00;
        }
    }
	parse_frame(myBuffer);

}