#include "grinder_timer.hpp"

ISR(BUTTON_PCI_VECTOR)
{
	buttons.disable();	// Disable button pin-change interrupt while this ISR is executed..

	_delay_ms(BUTTON_DEBOUNCE_MS);	// Wait for the button de-bounce duration.

	if	(buttons.state(BUTTON_UP))	handle_up_down(BUTTON_UP);
	else if	(buttons.state(BUTTON_DOWN))	handle_up_down(BUTTON_DOWN);
	else if (buttons.state(BUTTON_LEFT))	handle_left_right(BUTTON_LEFT);
	else if (buttons.state(BUTTON_RIGHT))	handle_left_right(BUTTON_RIGHT);
	else if (buttons.state(BUTTON_GRIND))	grind(true);


	if(!grinding)
	{
		while (~BUTTON_PINS & BUTTON_MASK) {}	// Wait until all buttons are released.
		buttons.enable();	// Re-enable button pin-change interrupt.
	}
}

// Set the LED as either on, off or pulsing.
void led_control(uint8_t led_mode)
{
	pulse.disable();

	if(led_mode)
	{
		if(led_mode >> 1)	pulse.enable();
		else			led.set(LED_MAX_BRIGHTNESS);

		led.enable();
	}
	else	led.disable();
}

// Initiate or cease grinding.
void grind(bool grind)
{
	if(grind)
	{
		grinding = true;
		rtc.enable();
		RELAY_ON;
		led_control(LED_OFF);
	}

	else
	{
		grinding = false;
		rtc.disable();
		RELAY_OFF;

		_delay_ms(1000);

		led_control(LED_ON);
		counter = preset_timer[current_preset];
//		refresh_timer();
		buttons.enable();
	}
}

// Functioned called to update display when scrolling up or down.
void handle_up_down(uint8_t up_or_down)
{
	switch(up_or_down)
	{
		case BUTTON_UP:
			oled.test_pattern(0b01010101);
			led_control(LED_PULSE);
			break;
		case BUTTON_DOWN:
			oled.clear_screen();
			led_control(LED_OFF);
			break;
	}
}

// Functioned called to update display when scrolling left or right.
void handle_left_right(uint8_t left_or_right)
{
	switch(left_or_right)
	{
		case BUTTON_LEFT:
			current_preset--;
			if(current_preset > PRESET_D) current_preset = PRESET_D;
			break;

		case BUTTON_RIGHT:
			current_preset++;
			if(current_preset > PRESET_D) current_preset = PRESET_A;
			break;
	}

	counter = preset_timer[current_preset];
	refresh_timer();
	refresh_menu();

}

ISR(TIMER_INT_VECTOR)
{
	// Reverse the pulse direction at either end of the count.
	if ((led.get() == LED_MAX_BRIGHTNESS) || (led.get() == 0)) pulse_dir *= -1;

	// Update the led brightness.
	led.set(led.get() + pulse_dir);						
}


ISR(CLOCK_INT_VECTOR)
{
	if(counter > 0)	counter--;
	else		grind(false);

	refresh_timer();
}

void refresh_timer(void)
{
	unsigned char digits_string[6] = {'0', '0', '.', '0', '0', 0};
	digits_string[0] = (((counter >> 4) / 10) + '0');		// Convert counter value 10s to ascii.
	digits_string[1] = (((counter >> 4) % 10) + '0');		// Convert counter value 1s to ascii.
	digits_string[3] = (((uint16_t)(counter * 0.625) % 10) + '0');	// Convert counter value 10ths to ascii.
	digits_string[4] = (((uint16_t)(counter * 6.25) % 10) + '0');	// Convert counter value 100ths to ascii.

	oled.print_string(digits_string, DSEG7_Classic_Bold_32, 3, 5);

}

void refresh_menu(void)
{
	unsigned char preset_icons[5] = {'A','B','C','D',0};	// Default icons i.e. not selected.
	preset_icons[current_preset] += 4;			// Charaters E, F, G & H actually show as inverted A, B, C & D to identify selected preset.
	oled.print_string(preset_icons, Preset_Icons, 0, 34);

	oled.print_char(LEFT_ARROW, Arrows, 0, 19);
	oled.print_char(RIGHT_ARROW, Arrows, 0, 94);
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

	// Configure the relay pin as an output.
	RELAY_DDR |= (1 << RELAY_PIN);

	// Initialise the buttons (keypad class).
	buttons.init();	

	// Initialise then enable the led (pwm class).
	led.init();
//	led.enable();

	// Initialise then set the speed of the led pulse effect (timer class).
	pulse.init();
	pulse.set(LED_PULSE_SPEED);
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

	led_control(LED_PULSE);

	buttons.enable();

	while(1)
	{
		//Driven by interrupts.
	}

	return(7);
}
