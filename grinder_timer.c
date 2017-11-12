/* Blinker Demo */


//#define F_CPU 1000000


#include <avr/io.h>		/* Defines pins, ports, etc */
#include <util/delay.h>		/* Functions to waste time */

#include "USART.h"
//#include "SPI.h"



int main(void) {

	initUSART();

	//Test string for USART initialisation
	printString("\r\nGrinder_Timer\r\n");

	DDRB |= 0b00000001;

	while (1) {
		PORTB = 0b00000001;          /* Turn on first LED bit/pin in PORTB */
		_delay_ms(500);                                           /* wait */
		PORTB = 0b00000000;          /* Turn off all B pins, including LED */
		_delay_ms(2000);                                           /* wait */
	}                                                  /* End event loop */

return 0;                            /* This line is never reached */
}
