#include <avr/io.h>		/* Defines pins, ports, etc */
#include <util/delay.h>		/* Functions to waste time */

#include <avr/power.h>	//added to set clock prescaler in code

#include "USART.h"
#include "I2C.h"
#include "SDD1306.h"

void oled_test_pattern(void)
{
	unsigned char i,j,k;
	for (k=0;k<8;k++)
	{
		oled_set_address(0,k);
		j=0;
		for(i=0;i<128;i++)
		{
			j++;
			if(j==8){j=0;}
			oled_send_data(1 << j);
		}
	}
}

void oled_draw_box(void)
{
	oled_set_address(10,2);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_send_data(0xFF);
	oled_set_address(10,3);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
	oled_send_data(0x03);
}


int main(void)
{
	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz

	initUSART();
	//Test string for USART initialisation
	printString("\r\n___Grinder_Timer\r\n");

	i2c_init();

	oled_init();

	printString("got here");

	//oled_send_command(0xa5);
	oled_test_pattern();
	while (1)
	{		
		oled_test_pattern();
		_delay_ms(500);
		oled_clear_screen();
		_delay_ms(1000);
		oled_draw_box();
		_delay_ms(2000);
		
	}

return 0;                            /* This line is never reached */
}
