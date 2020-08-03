#include "grinder_timer.h"



ISR(BUTTON_PCI_VECTOR)
{
	buttons.disable();	//Disable button pin-change interrupt while this ISR is executed..

	_delay_ms(BUTTON_DEBOUNCE_MS);	//Wait for the button de-bounce duration.

	if (buttons.state(BUTTON_UP))
	{
		serial.print_string((char*)"UP\r\n");
//		BUTTON_PORT &= ~(1 << BUTTON_LED);
//		RELAY_PORT &= ~(1 << RELAY_PIN);
	}
	else if (buttons.state(BUTTON_DOWN))	serial.print_string((char*)"DOWN\r\n");
	else if (buttons.state(BUTTON_LEFT))	serial.print_string((char*)"LEFT\r\n");
	else if (buttons.state(BUTTON_RIGHT))	serial.print_string((char*)"RIGHT\r\n");
	else if (buttons.state(BUTTON_GRIND))
	{
		serial.print_string((char*)"GRIND\r\n");
//		BUTTON_PORT |= (1 << BUTTON_LED);
//		RELAY_PORT |= (1 << RELAY_PIN);
	}	

	while (~BUTTON_PINS & BUTTON_MASK) {}	//Wait until all buttons are released.

	buttons.enable();	//Re-enable button pin-change interrupt.

}



void hardware_init()
{
	serial.init();

	RELAY_DDR |= (1 << RELAY_PIN);

	buttons.init();
	buttons.enable();

	led.init();
	led.enable();

	sei();	//Enable all interrupts.
}



int main(void)
{

	hardware_init();

	serial.print_string((char*)"testing 123\r\n");

	uint8_t i=0;

	while (1)
	{
		_delay_ms(10);
		led.set(i);
		i++;
	}

	return 0;	//Never reached.
}
