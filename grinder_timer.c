#include <avr/io.h>		/* Defines pins, ports, etc */
#include <util/delay.h>		/* Functions to waste time */

#include <avr/power.h>	//added to set clock prescaler in code

#include "usart.h"
#include "i2c.h"
#include "ssd1306.h"

//Initialise hardware (AVR peripherals and OLED module).
void hardware_init(void)
{
	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz
	usart_init();				//Initialise AVR USART hardware.
	i2c_init();				//Initialise AVR TWI (I2C) hardware.
	oled_init();				//Initialise the OLED module.
}

int main(void)
{
	hardware_init();
	
	//Test string for USART initialisation
	usart_print_string("\r\ngrind(coffee);\r\n");

	while (1)
	{		
		oled_test_pattern();
		_delay_ms(500);
		oled_clear_screen();
		_delay_ms(1000);
		oled_draw_box(0,0,128,8);
		_delay_ms(500);
		oled_draw_box(7,1,112,6);
		_delay_ms(500);
		oled_draw_box(15,2,96,4);
		_delay_ms(500);
		oled_draw_box(23,3,80,2);
		_delay_ms(5000);
	}

	return 0;	//Should never reach this.
}
