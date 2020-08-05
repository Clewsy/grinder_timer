#include "grinder_timer.h"


ISR(BUTTON_PCI_VECTOR)
{
	buttons.disable();	// Disable button pin-change interrupt while this ISR is executed..

	_delay_ms(BUTTON_DEBOUNCE_MS);	// Wait for the button de-bounce duration.

	if (buttons.state(BUTTON_UP))
	{
		pulse.disable();
		led.disable();
		oled.test_pattern(0b01010101);
//		RELAY_PORT &= ~(1 << RELAY_PIN);
	}
	else if (buttons.state(BUTTON_DOWN))
	{
		pulse.disable();
		led.disable();
		oled.clear_screen();
	}
	else if (buttons.state(BUTTON_LEFT))
	{
		pulse.disable();
		led.disable();
		oled.map_bits(LOGO_CLEWS, sizeof(LOGO_CLEWS));
	}
	else if (buttons.state(BUTTON_RIGHT))
	{
		pulse.disable();
		led.disable();
		oled.map_bits(LOGO_HAD, sizeof(LOGO_HAD));
	}
	else if (buttons.state(BUTTON_GRIND))
	{
		pulse.enable();
		led.enable();
//		RELAY_PORT |= (1 << RELAY_PIN);
	}	

	while (~BUTTON_PINS & BUTTON_MASK) {}	// Wait until all buttons are released.

	buttons.enable();	// Re-enable button pin-change interrupt.

}

ISR(TIMER_INT_VECTOR)
{
	if ((led.get() == MAX_BRIGHTNESS) || (led.get() == 0)) pulse_dir *= -1;	//Reverse the pulse direction at either end of the count.
	led.set(led.get() + pulse_dir);						//Update the led brightness.
}

void hardware_init()
{

	// Initialsie serial input/output (USART class).
//	serial.init();
//	serial.print_string((char*)"testing 123\r\n");

	RELAY_DDR |= (1 << RELAY_PIN);

	// Initialise then enable the buttons (keypad class).
	buttons.init();	
	buttons.enable();

	// Initialise then enable the led (pwm class).
	led.init();
	led.enable();

	// Initialise then enable the led pulse effect (timer class).
	pulse.init();
	pulse.enable();

	// Initialise the OLED display and show a splash-screen of sorts.
	oled.init();
	oled.test_pattern(0b00110011, 1);
	oled.clear_screen();
	oled.draw_box(0, 0, 8, 128);
	oled.print_string((unsigned char*)"grind(coffee);", Roboto_Black_12, 3, 26);
	oled.scroll(SCROLL_DN, 8, 1);
	oled.scroll(SCROLL_UP, 8, 1);
	oled.scroll(SCROLL_DN, 4, 2);
	oled.scroll(SCROLL_UP, 4, 2);
	oled.scroll(SCROLL_DN, 2, 4);
	oled.scroll(SCROLL_UP, 2, 4);
	oled.scroll(SCROLL_DN, 1, 8);
	oled.scroll(SCROLL_UP, 1, 8);

	// Globally enable all interrupts.
	sei();
}



int main(void)
{
	hardware_init();

	while(1)
	{
		//Driven by interrupts.
	}

	return(7);
}
