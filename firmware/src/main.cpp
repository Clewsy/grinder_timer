#include "grinder_timer.hpp"

ISR(BUTTON_PCI_VECTOR)
{
	buttons.disable();	// Disable button pin-change interrupt while this ISR is executed..

	_delay_ms(BUTTON_DEBOUNCE_MS);	// Wait for the button de-bounce duration.


	if (buttons.state(BUTTON_UP))
	{
		oled.test_pattern(0b01010101);
//		RELAY_PORT &= ~(1 << RELAY_PIN);
	}
	else if (buttons.state(BUTTON_DOWN))
	{
		oled.clear_screen();
		pulse.disable();
		led.disable();
		rtc.enable();
	}
	else if (buttons.state(BUTTON_LEFT))
	{
		current_preset--;
		if(current_preset > PRESET_D) current_preset = PRESET_D;

		refresh_preset_menu();
	}
	else if (buttons.state(BUTTON_RIGHT))
	{
		current_preset++;
		if(current_preset > PRESET_D) current_preset = PRESET_A;

		refresh_preset_menu();
	}
	else if (buttons.state(BUTTON_GRIND))
	{
		rtc.disable();
		led.enable();
		pulse.enable();
//		RELAY_PORT |= (1 << RELAY_PIN);
	}	

	while (~BUTTON_PINS & BUTTON_MASK) {}	// Wait until all buttons are released.

	buttons.enable();	// Re-enable button pin-change interrupt.

}


ISR(TIMER_INT_VECTOR)
{
	if ((led.get() == MAX_BRIGHTNESS) || (led.get() == 0)) pulse_dir *= -1;	// Reverse the pulse direction at either end of the count.
	led.set(led.get() + pulse_dir);						// Update the led brightness.
}


ISR(CLOCK_INT_VECTOR)
{
	counter++;
//	if(counter == 480) counter = 0;		// Timer resets at 480 counts = 30 seconds.
	if(counter == 1600) counter = 0;	// Timer resets at 480 counts = 100 seconds.

	unsigned char digits_string[6] = {'0', '0', '.', '0', '0', 0};
	digits_string[0] = (((counter >> 4) / 10) + '0');		// Convert counter value 10s to ascii.
	digits_string[1] = (((counter >> 4) % 10) + '0');		// Convert counter value 1s to ascii.
	digits_string[3] = (((uint16_t)(counter * 0.625) % 10) + '0');	// Convert counter value 10ths to ascii.
	digits_string[4] = (((uint16_t)(counter * 6.25) % 10) + '0');	// Convert counter value 100ths to ascii.

//	oled.print_string((unsigned char*)"ABCDEFGH", Preset_Icons, 0, 0);
	oled.print_string(digits_string, DSEG7_Classic_Bold_32, 3, 5);
}

void refresh_preset_menu(void)
{
	unsigned char preset_icons[5] = {'A','B','C','D',0};	// Default icons i.e. not selected.
	preset_icons[current_preset] += 4;			// Charaters E, F, G & H actually show as inverted A, B, C & D to identify selected preset.
	oled.print_string(preset_icons, Preset_Icons, 0, 5);
}

void splash(void)
{
//	oled.test_pattern(0b00110011, 1);
	oled.map_bits(LOGO_CLEWS, sizeof(LOGO_CLEWS));
	_delay_ms(500);
	oled.map_bits(LOGO_HAD, sizeof(LOGO_HAD));
	_delay_ms(500);
	oled.clear_screen();
	oled.draw_box(0, 0, 8, 128);
	oled.print_string((unsigned char*)"grind(coffee);", Roboto_Mono_12, 3, 15);
	oled.scroll(SCROLL_DN, 8, 1);
	oled.scroll(SCROLL_DN, 4, 2);
	oled.scroll(SCROLL_DN, 2, 4);
	oled.scroll(SCROLL_DN, 1, 8);
}

void hardware_init()
{
	// Initialsie serial input/output (USART class).
//	serial.init();
//	serial.print_string((char*)"grind(coffee);\r\n");

	RELAY_DDR |= (1 << RELAY_PIN);

	// Initialise the buttons (keypad class).
	buttons.init();	

	// Initialise then enable the led (pwm class).
	led.init();
//	led.enable();

	// Initialise then enable the led pulse effect (timer class).
	pulse.init();
//	pulse.enable();

	// Initialise the real-time clock (clock class).
	rtc.init();

	// Initialise the OLED display (sh1106 class)
	oled.init();

	// Globally enable all interrupts.
	sei();
}



int main(void)
{
	hardware_init();

	splash();

	buttons.enable();

	while(1)
	{
		//Driven by interrupts.
	}

	return(7);
}
