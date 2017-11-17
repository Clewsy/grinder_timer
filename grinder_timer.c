#include <avr/io.h>		/* Defines pins, ports, etc */
#include <util/delay.h>		/* Functions to waste time */

#include <avr/power.h>	//added to set clock prescaler in code

#include "usart.h"
#include "i2c.h"
#include "ssd1306.h"
#include "rtc.h"

#define RELAY_PORT		PORTC
#define RELAY_DDR		DDRC
#define RELAY_PIN			PC0

#define BUTTON_PORT		PORTB
#define BUTTON_DDR		DDRB
#define BUTTON_GRIND		PB0

//The following interrupt subroutine will be triggered every 1/16th of a second.
uint8_t global_seconds = 0;	//globally accessible variable used to count seconds.
uint16_t global_sixteenths = 0;	//globally accessible variable used to count sixteenths of a second.i
uint16_t global_timer_setpoint = 192;
ISR(TIMER2_OVF_vect)
{
	global_sixteenths--;
	if(global_sixteenths > 60000)		//If global_sixteenths has underflowed.
	{
		RELAY_PORT &= ~(1 << RELAY_PIN);
		global_sixteenths = 0;
		rtc_disable();	//Reset to 0.
	}
	global_seconds=(global_sixteenths>>4);	//Bitshift right four digits is equivalent to dividing by sixteen.

	//Print some information across serial for debugging.
	//usart_print_string("\r");			//Reset to the start of the line.
	//usart_print_byte(global_seconds);		//Convert the seconds integer to ascii characters and print.
	//usart_print_string(".");			//Print a dividing decimal point.
	//usart_print_byte(global_sixteenths*6.25);	//Print the decimal equivalent of sixteenths of a second.

	//Print some information to the oled display for debugging.
	oled_set_address(44,4);
	oled_type_byte(global_seconds);
	oled_type_char('.');
	oled_type_byte(global_sixteenths*6.25);
}

//The following interrupt sub-routine will be triggered every time there is a change in state of any button.
ISR(PCINT0_vect)
{
	global_sixteenths = global_timer_setpoint;
	RELAY_PORT |= (1 << RELAY_PIN);
	rtc_enable();
}//Interrupt flag PCIF0 (pin-change interrupt flag 0) on PCIFR (pin-change interrupt flag register) is automatically cleared upon exit of this ISR.


//Initialise hardware (AVR peripherals and OLED module).
void hardware_init(void)
{
	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz
	usart_init();				//Initialise AVR USART hardware.
	i2c_init();				//Initialise AVR TWI (I2C) hardware.
	oled_init();				//Initialise the OLED module.
	rtc_init();				//Initialise AVR Timer/Counter 2 hardware to operate asynchronously as a real time clock (for accurate timing).

	PCICR |= (1 << PCIE0);			//Enable Pin-Change Interrupt for pin-change int pins PCINT[0-7].  This includes all 5 buttons.
						//PCICR: Pin-Change Interrupt Control Register
						//PCIE[0-7]: Pin-Change Interrupt Enable 0-7
	PCMSK0 |= (1 << PCINT0);		//Enable Pin-Change Interrupt on PCINT[x] for PCINT pins to which the 5 buttons are connected.
						//PCMSK0: Pin-Change Mask Register 0
						//PCINT[0-7]: Pin-Change Interrupt [0-7]
	BUTTON_PORT |= (1 << BUTTON_GRIND);	//Enable pull-up resistors for all 5 buttons.

	
	RELAY_DDR |= (1 << RELAY_PIN);		//Set as an output the pin to which the relay is connected.

	sei();					//Global enable of interrupts.
}

int main(void)
{
	hardware_init();
	
	//Test string for USART initialisation
	usart_print_string("\n\r\ngrind(coffee);\r\n");
	oled_clear_screen();
	oled_set_address(8,0);
	oled_type_string("grind(coffee);");
	_delay_ms(500);

	while (1)
	{		
		//oled_test_pattern();
		//oled_draw_box(0,0,128,8);
	}

	return 0;	//Should never reach this.
}
