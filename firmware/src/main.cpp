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
//	oled.draw_box(0, 4, 26, 66);
}

void splash(void)
{
	oled.map_bits(LOGO_CLEWS, sizeof(LOGO_CLEWS));					// Show bitmap for a duration.
	_delay_ms(500);
	oled.map_bits(LOGO_HAD, sizeof(LOGO_HAD));					// Show bitmap for a duration.
	_delay_ms(500);
	oled.clear_screen();
	oled.draw_box(0, 0, 64, 128);							// Box around the outermost border of the oled.
	oled.print_string((unsigned char*)"grind(coffee);", Roboto_Mono_12, 3, 15);	// Text centered.

	// Scrolling animation, iterates 4 times:
	// 1st: 8 cycles, 1ms delay.
	// 2nd: 4 cycles, 2ms delay.
	// 3rd: 2 cycles, 4ms delay.
	// 4th: 1 cycle,  8ms delay.
	for(uint8_t i = 1; i < 16; i*=2) oled.scroll(SCROLL_DN, (8 / i), i);
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
