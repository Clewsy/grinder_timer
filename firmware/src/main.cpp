#include "grinder_timer.h"


ISR(BUTTON_PCI_VECTOR)
{
	buttons.disable();	//Disable button pin-change interrupt while this ISR is executed..

	_delay_ms(BUTTON_DEBOUNCE_MS);	//Wait for the button de-bounce duration.

	if (buttons.state(BUTTON_UP))
	{
		serial.print_string((char*)"UP\r\n");
		led.disable();
		oled.test_pattern();
//		RELAY_PORT &= ~(1 << RELAY_PIN);
	}
	else if (buttons.state(BUTTON_DOWN))
	{
		serial.print_string((char*)"DOWN\r\n");
		oled.clear_screen();
	}
	else if (buttons.state(BUTTON_LEFT))
	{
		serial.print_string((char*)"LEFT\r\n");
		oled.invert_screen(true);
		oled.map_bits(LOGO_CLEWS, sizeof(LOGO_CLEWS));
	}
	else if (buttons.state(BUTTON_RIGHT))
	{
		serial.print_string((char*)"RIGHT\r\n");
		oled.invert_screen(false);
		oled.map_bits(LOGO_HAD, sizeof(LOGO_HAD));
	}
	else if (buttons.state(BUTTON_GRIND))
	{
		serial.print_string((char*)"GRIND\r\n");
		led.enable();
//		RELAY_PORT |= (1 << RELAY_PIN);
	}	

	while (~BUTTON_PINS & BUTTON_MASK) {}	//Wait until all buttons are released.

	buttons.enable();	//Re-enable button pin-change interrupt.

}

ISR(TIMER_INT_VECTOR)
{
	if ((led.get() == MAX_BRIGHTNESS) || (led.get() == 0)) pulse_dir *= -1;	//Reverse the pulse direction at either end of the count.
	led.set(led.get() + pulse_dir);						//Update the led brightness.
}

void hardware_init()
{

	//Initialsie serial input/output (USART class).
	serial.init();
	serial.print_string((char*)"testing 123\r\n");

	RELAY_DDR |= (1 << RELAY_PIN);

	//Initialise then enable the buttons (keypad class).
	buttons.init();	
	buttons.enable();

	//Initialise then enable the led (pwm class).
	led.init();
	led.enable();

	//Initialise then enable the led pulse effect (timer class).
	pulse.init();
	pulse.enable();


//twi.init();
//oled.invert_screen(true);
//oled.test_pattern();
	//Enable all interrupts.
	sei();

oled.init();
oled.test_pattern();
oled.clear_screen();

}



int main(void)
{

	hardware_init();



	while (1)
	{
		//Driven by interrupts.
	}

	return 0;	//Never reached.
}
