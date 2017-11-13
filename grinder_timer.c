/* Blinker Demo */


//#define F_CPU 1000000


#include <avr/io.h>		/* Defines pins, ports, etc */
#include <util/delay.h>		/* Functions to waste time */

#include <avr/power.h>	//added to set clock prescaler in code

#include "USART.h"
#include "I2C.h"
#include "SDD1306.h"

#define OLED_CS		PB0	//OLED SSD1306 Chip Select (CS) pin connected to AVR Port B Pin 0
#define OLED_RST	PB1	//OLED SSD1306 Reset (RST) pin connected to AVR Port B Pin 1
#define OLED_DC		PB6	//OLED SSD1306 Data/Command (D/C) pin connected to AVR Port B Pin 6
#define OLED_SCLK	PB5	//OLED SSD1306 Clock (SCLK) pin connected to AVR Port B Pin 5
#define OLED_MOSI	PB3	//OLED SSD1306 Master Out Slave In (MOSI) pin connected to AVR Port B Pin 3
#define OLED_DDR	DDRB
#define OLED_PORT	PORTB


// Set the cursor position in a 16 COL * 8 ROW map.
void setXY(unsigned char row,unsigned char col)
{
	oled_send_command(0xb0+row);                //set page address
	oled_send_command(0x00+(8*col&0x0f));       //set low col address
	oled_send_command(0x10+((8*col>>4)&0x0f));  //set high col address
}



/*
// Prints a string in coordinates X Y, being multiples of 8.
// // This means we have 16 COLS (0-15) and 8 ROWS (0-7).
void sendStrXY( char *string, int X, int Y)
{
	setXY(X,Y);
	unsigned char i=0;
	while(*string)
	{
		for(i=0;i<8;i++)
		{
			SendChar(pgm_read_byte(myFont[*string-0x20]+i));
		}
		*string++;
	}
}
*/

void oled_clear_display(uint8_t dat)
{
	unsigned char i,k;
	for (k=0;k<8;k++)
	{
		setXY(k,0);
		{
			for(i=0;i<128;i++)
			{
				oled_send_data(dat);
			}
		}
	}
}

int main(void) {

	initUSART();

	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz

	//Test string for USART initialisation
	printString("\r\n___Grinder_Timer\r\n");

//	DDRC |= 0b00000001;

	i2c_init();

	oled_send_command(0xae);
	oled_send_command(0xd5);
	oled_send_command(0x80);
	oled_send_command(0xa8);
	oled_send_command(0x3f);
	oled_send_command(0xd3);
	oled_send_command(0x00);
	oled_send_command(0x40 | 0x0);
	oled_send_command(0x8d);
	oled_send_command(0x14);
	oled_send_command(0x20);
	oled_send_command(0x00);
//	oled_send_command(0xaf);


	oled_send_command(0xa0 | 0x1);
	oled_send_command(0xc8);
	oled_send_command(0xda);
	oled_send_command(0x12);
	oled_send_command(0x81);
	oled_send_command(0xcf);
	oled_send_command(0xd9);
	oled_send_command(0xf1);
	oled_send_command(0xdb);
	oled_send_command(0x40);
	oled_send_command(0xa4);
	oled_send_command(0xa6);
/*	oled_send_command(0x
	oled_send_command(0x
	oled_send_command(0x
	oled_send_command(0x
*/

oled_clear_display(0);

oled_send_command(0x2e);
oled_send_command(0x20);
oled_send_command(0x00);

setXY(0,0);
oled_send_command(0xaf);


oled_send_command(0xae);
oled_clear_display(0);
oled_send_command(0xaf);



printString("got here");

	//oled_send_command(0xa5);
	while (1)
	{
		uint8_t j;
		for(j=0;j<0b11111111;j++)
		{
			oled_clear_display(j);
			_delay_ms(20);
		}
	//	oled_send_command(0xae);
	//	_delay_ms(1000);
	//	oled_send_command(0xaf);
	//	_delay_ms(1000);
	}                                                  /* End event loop */

return 0;                            /* This line is never reached */
}
