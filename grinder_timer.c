#include "grinder_timer.h"


//The following interrupt subroutine will be triggered every 1/16th of a second.
//uint8_t global_seconds = 0;	//globally accessible variable used to count seconds.
ISR(TIMER2_OVF_vect)
{
	global_sixteenths--;
	if(global_sixteenths > 60000)		//If global_sixteenths has underflowed.
	{
		RELAY_PORT &= ~(1 << RELAY_PIN);
		global_sixteenths = 0;
		rtc_disable();
		_delay_ms(1000);
		global_sixteenths = global_timer_setpoint;
	}
	display_clock();
}

//The following interrupt sub-routine will be triggered every time there is a change in state of any button.
ISR(BUTTON_PCI_VECTOR)
{
	//_delay_ms(50);
	if((BUTTON_PINS & (1 << BUTTON_GRIND)) == 0)	//If the grind button is pressed
	{
		global_sixteenths = global_timer_setpoint;
		RELAY_PORT |= (1 << RELAY_PIN);
		rtc_enable();
	}
}//Interrupt flag PCIF0 (pin-change interrupt flag 0) on PCIFR (pin-change interrupt flag register) is automatically cleared upon exit of this ISR.

//This function displays the current value of the timer by converting the quantity of "global_sixteenths" to seconds.
//The digits are displayed with the large seven-segment style font.
void display_clock(void)
{
	oled_type_digit_large(((global_sixteenths >> 4) / 10), 4, 2);			//Tens of seconds. RShift 4 bits to divide by 16. Print with top left at column 4, page 2.
	oled_type_digit_large(((global_sixteenths >> 4) % 10), 28, 2);			//Ones of seconds. % (i.e. mod) gives remainder after dividing.
	oled_type_digit_large(11, 52, 2);						//The eleventh character in this font is a period ('.').  This serves as a separator.
	oled_type_digit_large(((uint8_t)(global_sixteenths*0.625) % 10), 76, 2);	//Tenths of seconds. Note division and modulus operators ignore fractions.
	oled_type_digit_large(((uint16_t)(global_sixteenths*6.25) % 10) , 100, 2);	//Hudredths of a second. Note modulus operation (%) only works on integers.
}


//Initialise hardware (AVR peripherals and OLED module).
void hardware_init(void)
{
	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz
	usart_init();				//Initialise AVR USART hardware.
	i2c_init();				//Initialise AVR TWI (I2C) hardware.
	oled_init();				//Initialise the OLED module.
	rtc_init();				//Initialise AVR Timer/Counter 2 hardware to operate asynchronously as a real time clock (for accurate timing).

	//Setup inputs and outputs.
	PCICR |= (1 << BUTTON_PCIE);		//Enable Pin-Change Interrupt for pin-change int pins PCINT[0-7].  This includes all 5 buttons.
						//PCICR: Pin-Change Interrupt Control Register
						//PCIE0: Pin-Change Interrupt Enable 0 (Enables PCINT[0-7] i.e. PB[7-0])
						//PCIE1: Pin-Change Interrupt Enable 1 (Enables PCINT[14-8] i.e. PC[6-0])
						//PCIE2: Pin-Change Interrupt Enable 2 (Enables PCINT[23-16] i.e. PD[7-0])
	BUTTON_PCMSK |= ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT));
						//Enable Pin-Change Interrupt on PCINT[x] for PCINT pins to which the 5 buttons are connected.
						//PCMSK0: Pin-Change Mask Register 0 (For PCINT[0-7] i.e. PB[7-0])
						//PCMSK1: Pin-Change Mask Register 1 (For PCINT[14-8] i.e. PC[6-0])
						//PCMSK2: Pin-Change Mask Register 2 (For PCINT[23-16] i.e. PD[7-0])
						//PCINT[0-7]: Pin-Change Interrupt [0-7]. Note PCINT[7-0]: PB[7-0]
						//PCINT[14-8]: Pin-Change Interrupt [14-8]. Note PCINT[7-0]: PC[6-0]
						//PCINT[23-16]: Pin-Change Interrupt [23-16]. Note PCINT[7-0]: PD[7-0]
	BUTTON_PORT |= ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT));
						//Enable pull-up resistors for all 5 buttons.
	RELAY_DDR |= (1 << RELAY_PIN);		//Set as an output the pin to which the relay is connected.

	sei();					//Global enable of interrupts.
}

int main(void)
{
	hardware_init();

	//Test string for USART initialisation
	usart_print_string("\n\r\ngrind(coffee);\r\n");
	oled_clear_screen();
	oled_set_address(0,0);
	oled_type_string("grind(coffee);");

	//Set an initial boot-up timer value and display on-screen.
	global_sixteenths = global_timer_setpoint;
	display_clock();

	while (1)
	{
		//Everything is interrupt driven.
	}

	return 0;	//Should never reach this.
}
