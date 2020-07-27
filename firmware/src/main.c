#include "usart.h"

int main(void)
{
//	//Initialise AVR peripherals and external devices (OLED).
//	hardware_init();
//
//	//Test string for USART initialisation.
//	usart_print_string("\n\r\ngrind(coffee);\r\n");
//
//	//Test the oled display.
//	display_splash_screen();
//
//	//Initialise the presets (pull from eeprom and validate).
//	presets_init();
//
//	//Set an initial boot-up timer value (preset A) and display menu and timer on-screen.
//	global_sixteenths = global_presets[global_current_preset];
//	display_clock();
//	display_menu(global_current_preset);
//
//	//Now ready to detect button presses.
//	PCICR |= (1 << BUTTON_PCIE);	//Enable pin-change interrupt for buttons.

usart_init();

	while (1)
	{
		usart_print_string("testink 123 ");
		//Everything is interrupt driven.
	}

	return 0;	//Never reached.
}
