#include <avr/io.h>		/* Defines pins, ports, etc */
#include <util/delay.h>		/* Functions to waste time */

#include <avr/power.h>	//added to set clock prescaler in code

#include "usart.h"
#include "i2c.h"
#include "ssd1306.h"
#include "rtc.h"

//The following interrupt subroutine will be triggered every 1/16th of a second.
uint8_t global_seconds = 0;	//globally accessible variable used to count seconds.
uint8_t global_sixteenths = 0;	//globally accessible variable used to count sixteenths of a second.
ISR(TIMER2_OVF_vect)
{
	global_sixteenths++;			//increment the sixteenths variable.
	if(global_sixteenths == 16)		//this indicates a full second has elapsed.
	{
		global_sixteenths = 0;		//reset the sixteenths counter.
		global_seconds++;		//increment the seconds counter.
	}
	//Print some information across serial for debugging.
	usart_print_string("\r");			//Reset to the start of the line.
	usart_print_byte(global_seconds);		//Convert the seconds integer to ascii characters and print.
	usart_print_string(".");			//Print a dividing decimal point.
	usart_print_byte(global_sixteenths*6.25);	//Print the decimal equivalent of sixteenths of a second.
}



//Initialise hardware (AVR peripherals and OLED module).
void hardware_init(void)
{
	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz
	usart_init();				//Initialise AVR USART hardware.
	i2c_init();				//Initialise AVR TWI (I2C) hardware.
	oled_init();				//Initialise the OLED module.
	rtc_init();				//Initialise AVR Timer/Counter 2 hardware to operate asynchronously as a real time clock (for accurate timing).
	sei();					//Global enable of interrupts.
}

int main(void)
{
	hardware_init();
	
	//Test string for USART initialisation
	usart_print_string("\n\r\ngrind(coffee);\r\n");

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
