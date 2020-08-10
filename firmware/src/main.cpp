#include "grinder_timer.hpp"

// A pin-change on any keypad button triggers this sub-routine.
ISR(BUTTON_PCI_VECTOR)
{
	buttons.disable();		// Disable button pin-change interrupt while this ISR is executed..
	_delay_ms(BUTTON_DEBOUNCE_MS);	// Wait for the button de-bounce duration.

	if(grinding && buttons.any())	grind(false);	// If grinding, cancel grind with any button.
	else if(!grinding)
	{
		if	(buttons.check(BUTTON_UP))	handle_up_down(UP);		// Increase current preset timer.
		else if	(buttons.check(BUTTON_DOWN))	handle_up_down(DOWN);		// Decrease current preset timer.
		else if (buttons.check(BUTTON_LEFT))	handle_left_right(LEFT);	// Select preset to the left.
		else if (buttons.check(BUTTON_RIGHT))	handle_left_right(RIGHT);	// Select preset to the right.
		else if (buttons.check(BUTTON_GRIND))	grind(true);			// Start grinding.
	}

	buttons.enable();					// Re-enable button pin-change interrupt.
}

// An overflow of the timer output compare register triggers this interrupt sub-routine.
// Configured to vary the LED brightness at desired interval to create a pulsing effect.
ISR(TIMER_INT_VECTOR)
{
	if ((led.get() == LED_MAX_BRIGHTNESS) || (led.get() == 0)) pulse_dir *= -1;	// Reverse the pulse direction at either end of the count.
	led.set(led.get() + pulse_dir);							// Update the led brightness.
}

// An overflow of the timer counter register triggers this interrupt sub-routine.
// Configured to use external 32.768kHz crystal and therefore provide accurate timing in sixteenths of a second.
ISR(CLOCK_INT_VECTOR)
{
	if(counter > 0)	counter--;	// Count down to zero.
	else		grind(false);	// If reached zero, stop grinding.

	refresh_timer();		// Update the display with the remaining countdown duration.
}

// Set the LED as either on, off or pulsing.
// Valid values for led_mode are:
// LED_OFF	00
// LED_ON	01
// LED_PULSE	10
void led_control(uint8_t led_mode)
{
	pulse.disable();	// Start by assuming LED_OFF.
	led.disable();

	if(led_mode)		// LED_ON or LED_PULSE.
	{
		if(led_mode >> 1)	pulse.enable();			// LED_PULSE.
		else			led.set(LED_MAX_BRIGHTNESS);	// LED_ON.
		led.enable();
	}
}

// Called to alter the  value of the currently selected preset.
// Can be adjusted up or down in increments of 0.25 seconds.
// Oled is also refreshed accordingly.
void change_preset_value(int8_t up_or_down)
{
	preset_timer[current_preset] += (up_or_down * 4);							// Either +=4 or -=4.
	if(preset_timer[current_preset] > PRESET_MAX_VALUE) preset_timer[current_preset] = PRESET_MAX_VALUE;	// Upper limit.
	if(preset_timer[current_preset] < PRESET_MIN_VALUE) preset_timer[current_preset] = PRESET_MIN_VALUE;	// Lower limit.

	counter = preset_timer[current_preset];	// Update the timer value.
	refresh_timer();			// Update the timer display.
}

// Handle user pressing and releasing, or pressding and holding the up or down buttons.
// Up and down buttons are used to adjust the value of the currently selected preset.
void handle_up_down(int8_t up_or_down)
{
	change_preset_value(up_or_down);	// Update the preset value and refresh the display.

	uint16_t hold_timer = BUTTON_LONG_PRESS;
	while((buttons.any()) && ((hold_timer--) > 0)) _delay_ms(1);	// Wait until button is released or long-press duration elapses.

	while(buttons.any())	// Until the button is released.
	{
		change_preset_value(up_or_down);
		_delay_ms(BUTTON_FAST_CHANGE);		
	}
}

// Functioned called to update display when scrolling left or right.
void handle_left_right(int8_t left_or_right)
{
	current_preset += (left_or_right);	// Equivalent to either ++ or --.

	if(current_preset == (PRESET_D + 1))	current_preset = PRESET_A;	// D->A
	if(current_preset == 0xFF)		current_preset = PRESET_D;	// D<-A

	counter = preset_timer[current_preset];	// Update the timer value.
	refresh_timer();			// Update the timer display.
	refresh_menu();				// Update the presets menu.

	while(buttons.any()) {}	// Wait until the button is released.
}

// Initiate or cease grinding.
void grind(bool grind)
{
	grinding = grind;	// Set or clear the grinding flag.

	if(grind)
	{
		rtc.enable(true);	// Enable the countdown.
//		RELAY_ON;
		led_control(LED_OFF);
	}

	else
	{
		rtc.enable(false);	// Disable the countdown.
//		RELAY_OFF;

		_delay_ms(RELAY_RESET_DELAY);

		led_control(LED_ON);
		counter = preset_timer[current_preset];
		refresh_timer();
	}
}

// Update the clock/timer section of the OLED.
void refresh_timer(void)
{
	unsigned char digits_string[6] = {'0', '0', '.', '0', '0', 0};	// Want to display the current timer value as ##.## seconds.
	digits_string[0] = (((counter >> 4) / 10) + '0');		// Convert counter value 10s to ascii.
	digits_string[1] = (((counter >> 4) % 10) + '0');		// Convert counter value 1s to ascii.
	digits_string[3] = (((uint16_t)(counter * 0.625) % 10) + '0');	// Convert counter value 10ths to ascii.
	digits_string[4] = (((uint16_t)(counter * 6.25) % 10) + '0');	// Convert counter value 100ths to ascii.

	oled.print_string(digits_string, DSEG7_Classic_Bold_32, 3, 5);	// Write the array to the display.
}

// Update the preset selection menu section of the OLED.
void refresh_menu(void)
{
	unsigned char preset_icons[5] = {'A','B','C','D',0};	// Default icons i.e. not selected.
	preset_icons[current_preset] += 4;			// Charaters E, F, G & H actually show as inverted A, B, C & D to identify selected preset.
	oled.print_string(preset_icons, Preset_Icons, 0, 34);	// Print the preset icons string to the OLED.

	oled.print_char(LEFT_ARROW, Arrows, 0, 19);		// Print a left-pointing arrow to the left of the preset icons.
	oled.print_char(RIGHT_ARROW, Arrows, 0, 94);		// Print a right-pointing arrow to the right of the preset icons.
}

// Display a silly "animation" when powered on.
void splash(void)
{
	oled.map_bits(LOGO_CLEWS, sizeof(LOGO_CLEWS));					// Show bitmap for a duration.
	_delay_ms(250);
	oled.map_bits(LOGO_HAD, sizeof(LOGO_HAD));					// Show bitmap for a duration.
	_delay_ms(250);
	oled.clear_screen();
	oled.draw_box(0, 0, 64, 128);							// Box around the outermost border of the oled.
	oled.print_string((unsigned char*)"grind(coffee);", Roboto_Mono_12, 3, 15);	// Text centered (roughly).
	for(uint8_t i = 1; i < 16; i*=2) oled.scroll(SCROLL_DN, (8 / i), i);		// Scrolling animation, iterates 4 times:
											// 1st: 8 cycles, 1ms delay.
											// 2nd: 4 cycles, 2ms delay.
											// 3rd: 2 cycles, 4ms delay.
											// 4th: 1 cycle,  8ms delay.
	_delay_ms(1000);
	oled.clear_screen();
	refresh_menu();									// Show the preset menu.
	refresh_timer();								// Show the current preset value.
}

// Initialise the various hardware peripherals.
void hardware_init()
{
	// Initialsie serial input/output (USART class).
//	serial.init();
//	serial.print_string((char*)"grind(coffee);\r\n");

	// Configure the relay pin as an output.
	RELAY_DDR |= (1 << RELAY_PIN);

	// Initialise the buttons (keypad class).
	buttons.init();	

	// Initialise the led (pwm class for variable brightness).
	led.init();

	// Initialise then set the speed of the led pulse effect (timer class).
	pulse.init();
	pulse.set(LED_PULSE_SPEED);

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
