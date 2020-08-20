#include "grinder_timer.h"

//The following interrupt subroutine will be triggered every 1/16th of a second.  This timer is configured to use an external 32.768kHz oscillator.
//so that accurate timing can be achieved for the actual grinder countdown timer.
ISR(TIMER2_OVF_vect)
{
	global_sixteenths--;			//Decrement the counter.
	if(global_sixteenths > 60000)		//If global_sixteenths has underflowed (i.e. countdown timer has elapsed).
	{
		RELAY_PORT &= ~(1 << RELAY_PIN);				//Switch the grinder motor relay back off.
		rtc_disable();							//Disable the timer.
		presets_update_eeprom();					//If the preset changed, save to eeprom (after grind to not increase duration).
		_delay_ms(1000);						//Pause for one second with the timer display at zero.
		global_sixteenths = global_presets[global_current_preset];	//Reset the countdown duration to the current preset.
		PCICR |= (1 << BUTTON_PCIE);					//Effectively re-enable the buttons.
	}
	display_clock();	//Refresh the timer display to the value according to the current preset.
}

//The following interrupt sub-routine will be triggered every time there is a change in state of any button.
//ISR has 6 sections, one for each button, and one for waking the OLED from "sleep".
ISR(BUTTON_PCI_VECTOR)
{
	_delay_ms(BUTTON_DEBOUNCE_DURATION);	//wait for DEBOUNCE_DURATION milliseconds to mitigate effect of switch bounce.

	//If statement so that any button pressed wakes the oled if it is "sleeping" but does nothing else.
	if(global_oled_sleep_flag)			//If the oled is "sleeping" (i.e. turned off by the TC0 overflow interrupt)
	{						//then we just want to turn the screen back on with any button press.
		reset_oled_sleep_timer();		//Call the oled sleep timer reset function.
		while(~(BUTTON_PINS) & ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT))){}
							//Wait until all buttons are released (avoid running straight through to button-specific action).
		_delay_ms(BUTTON_DEBOUNCE_DURATION);	//wait for DEBOUNCE_DURATION milliseconds to mitigate effect of switch bounce at release.
		return;					//No other action regardless of button pressed.
	}

	reset_oled_sleep_timer();	//Call the oled sleep timer reset function.  If any button is pressed, the oled turning off should be delayed.
					//Don't want this before the if() above as the reset function clears the oled_sleep_flag

	uint16_t long_press_counter = BUTTON_HOLD_DURATION;	//Variable used to determine if a button is held for a specified duration.
	//Switch statement to run a specific case for each button.
	switch(~BUTTON_PINS & ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT)))
	{
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//GRIND BUTTON - If statement to capture actuation of the grind button.
		//This button triggers the relay and begins counting down the timer for the duration in accordance with the selected preset.
		case (1 << BUTTON_GRIND) :
			PCICR &= ~(1 << BUTTON_PCIE);	//Effectively disable the buttons while grinding (will be re-enabled at timer overflow ISR).
			RELAY_PORT |= (1 << RELAY_PIN);	//Activate the grinder motor relay.
			rtc_enable();			//Enable the countdown timer.
			return;				//The grinder motor should now be running.  Exit from the ISR.

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//LEFT BUTTON - If statement to capture actuation of the left button.
		//This button changes the selected preset scrolls backwards through them (A->D->C->B->A)
		case (1 << BUTTON_LEFT) :
			global_current_preset--;			//Decrement the currently selected preset.
			if(global_current_preset == 255)		//Rollunder from 0 to 4 (A to D)
			{
				global_current_preset = PRESET_D;
			}
			presets_update_eeprom();			//If the previous preset was changed, this will save it to eeprom.
			break;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//RIGHT BUTTON - If statement to capture actuation of the right button.
		//This button changes the selected preset scrolls forwards through them (A->B->C->D->A)
		case (1 << BUTTON_RIGHT) :
			global_current_preset++;			//Increment the currently selected preset.
			if(global_current_preset == 4)			//Rollover from 4 to 0 (D to A)
			{
				global_current_preset = PRESET_A;
			}
			presets_update_eeprom();			//If the previous preset was changed, this will save it to eeprom.
			break;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//UP BUTTON
		//If statement to capture actuation of the up button.
		//This button adjusts upwards the duration of the selected preset.
		case (1 << BUTTON_UP) :
			increment_current_preset();	//Increment the current preset value and check for rollover.
			display_clock();		//Update the timer display to show the preset in seconds.

			//Following while loop tests if the button is held for a specified duration.  Exits if time elapsed or button released.
			while((~BUTTON_PINS & (1 << BUTTON_UP)) && (long_press_counter > 0))
			{
				long_press_counter--;
				_delay_ms(1);
			}

			//If the button is still held after the specified duration, a fast increment runs until the button is released.
			while(~BUTTON_PINS & (1 << BUTTON_UP))			//Run this loop until the button is released.  Fast increment.
			{
				increment_current_preset();			//Increment the current preset value and check for rollover.
				display_clock();				//Update the timer display to show the preset in seconds.
				_delay_ms(BUTTON_FAST_CHANGE_DURATION);		//Brief delay between increments.
			}
			break;

		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
		//DOWN BUTTON
		//If statement to capture actuation of the down button.
		//This button adjusts downwards the duration of the selected preset.
		case (1 << BUTTON_DOWN) :
			decrement_current_preset();	//Decrement the current preset value and check for rollunder.
			display_clock();		//Update the timer display to show the preset in seconds.

			//Following while loop tests if the button is held for a specified duration.  Exits if time elapsed or button released.
			while((~BUTTON_PINS & (1 << BUTTON_DOWN)) && (long_press_counter > 0))
			{
				long_press_counter--;
				_delay_ms(1);
			}

			//If the button is still held after the specified duration, a fast decrement runs until the button is released.
			while(~BUTTON_PINS & (1 << BUTTON_DOWN))		//Run this loop until the button is released.  Fast increment.
			{
				decrement_current_preset();			//Decrement the current preset value and check for rollunder.
				display_clock();				//Update the timer display to show the preset in seconds.
				_delay_ms(BUTTON_FAST_CHANGE_DURATION);		//Brief delay between increments.
			}
			break;
	}

	display_menu(global_current_preset);				//Update the menu to indicate which preset is selected.
	global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
	display_clock();						//Update the timer display to show the preset in seconds.
	while(~(BUTTON_PINS) & ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT))){}
									//Wait until all buttons are released.

}//Interrupt flag PCIF0 (pin-change interrupt flag 0) on PCIFR (pin-change interrupt flag register) is automatically cleared upon exit of this ISR.

//This function is called when the up button is pressed.
//It increments the value of the current preset which represents a countdown duration in sixteenths of a second.
void increment_current_preset(void)
{
	global_presets[global_current_preset] = (global_presets[global_current_preset] + 4);	//Increment the preset value by 4 sixteenths (0.25s).
	if(global_presets[global_current_preset] > PRESET_MAX)					//Cap the maximum value.
	{
		global_presets[global_current_preset] = PRESET_MAX;
	}
	global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding and/or display update.
}

//This function is called when the down button is pressed.
//It decrements the value of the current preset which represents a countdown duration in sixteenths of a second.
void decrement_current_preset(void)
{
	global_presets[global_current_preset] = (global_presets[global_current_preset] - 4);	//Deccrement the preset value by 4 sixteenths (0.25s).
	if(global_presets[global_current_preset] > 65000)					//Indicates rollunder from zero.
	{
		global_presets[global_current_preset] = 0;
	}
	global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding and/or display update.

}

//Timer/Counter 0 overflow interrupt vector.
//TC0 is used as a simple counter prescaled by 1024 (~7812Hz) that triggers this interrupt at overflow.
//The interrupt function just increments a counter, but when the counter reaches a nominated value, the OLED is put to "sleep" (screen turned off).
ISR(TIMER0_OVF_vect)
{
	global_oled_sleep_timer++;			//Increment the OLED sleep timer counter.
	if(global_oled_sleep_timer > OLED_SLEEP_TIMER)	//If the counter reaches a determined value.
	{
		global_oled_sleep_timer = 0;		//Reset the counter.
		global_oled_sleep_flag = OLED_SLEEP_ON;	//Flag that the OLED is "sleeping".
		oled_send_command(SET_DISPLAY_OFF);	//Turn off the OLED.
	}
}

//This function resets the counter that would otherwise eventually put the OLED to "sleep".
//Whenever called, this function will effectively reset a countdown (countup) to turning off the OLED.
void reset_oled_sleep_timer(void)
{
	oled_send_command(SET_DISPLAY_ON);		//Ensure the OLED is turned on
	global_oled_sleep_flag = OLED_SLEEP_OFF;	//Flag that the OLED is not "sleeping".
	global_oled_sleep_timer = 0;			//Reset the OLED sleep timer counter.
	return;
}

//This function displays the current value of the timer by converting the quantity of "global_sixteenths" to seconds rounded to two decimal places.
//The digits are displayed with the large seven-segment style font (font_7_seg_digits).
void display_clock(void)
{
	oled_type_digit_large(((global_sixteenths >> 4) / 10), 4, 2);			//Tens of seconds. RShift 4 bits to divide by 16.
	oled_type_digit_large(((global_sixteenths >> 4) % 10), 28, 2);			//Ones of seconds. % (i.e. modulus) gives remainder after dividing.
	oled_type_digit_large(11, 52, 2);						//The 11th character in this font (period '.') serves as a separator.
	oled_type_digit_large(((uint16_t)(global_sixteenths*0.625) % 10), 76, 2);	//Tenths of seconds. Note / and % operators ignore fractions.
	oled_type_digit_large(((uint16_t)(global_sixteenths*6.25) % 10), 100, 2);	//Hudredths of a second. Note mod operation (%) only works on integers.
}

//Draws the preset menu at the top of the screen.  Presets represented by A, B, C and D.
//The currently selected preset is indicated by a box around the letter.
void display_menu(uint8_t selected_preset)
{
	uint8_t i;
	for(i=0;i<4;i++)	//Repeat four times for presets A, B, C and D.
	{
		//If statement will usually just draw the characters A to D, except in the case of the selected_preset character.
		//By adding 4 to the font map, the alternate character Will be selected (same character but within a box).
		if(i == selected_preset)
		{
			oled_type_menu_char(i+4,((i*24)+22),0);
		}
		else
		{
			oled_type_menu_char(i,((i*24)+22),0);
		}
	}
}

//A simple function to briefly display a splash screen on power-up.
void display_splash_screen(void)
{
	oled_clear_screen();			//Ensure the screen is clear.
	oled_draw_box(0,0,128,8);		//Draw a box around the outermost perimeter of the OLED.
	oled_set_address(22,3);			//Set the start address before typing a string.  This location will center a 14-char string.
	oled_type_string("grind(coffee);");	//Type a string.
	_delay_ms(2000);			//Pause for a couple of seconds before resuming boot.
	oled_clear_screen();			//Clear the screen again.
}

//This function is run on start-up to pull the presets saved to EEPROM and make them easily (globally) accessible.
//It will also set the presets to zero if they are not a valid number (such as after flashing a new AVR or changing the eeprom storage address).
//To be valid, the preset must be a multiple of 4 (only possible increments/decrements of sixteenths) and also less than PRESET_MAX.
void presets_init(void)
{
	uint8_t i;
	for(i=0;i<4;i++)	//Run through for each of the four presets.
	{
		global_presets[i] = eeprom_read_word((uint16_t *) (PRESET_EEPROM_ADDRESS + (2*i)));	//Read the current preset from eeprom
		if ((global_presets[i] > PRESET_MAX) || (global_presets[i] % 4))			//Check if it's not valid
		{
			global_presets[i] = 0;								//Reset preset to 0
			eeprom_update_word((uint16_t *) (PRESET_EEPROM_ADDRESS + (2*i)), 0);		//Save valid value (0) to eeprom.
		}
	}
}

//This function compares the presets in use to those stored in eeprom.  If they differ then the eeprom is updated.
//It will be called when changing between presets or starting the grinder as these are the times the presets have possibly been changed.
void presets_update_eeprom(void)
{
	uint8_t i;
	for(i=0;i<4;i++)		//Run through for each of the four presets.
	{
		eeprom_update_word((uint16_t *) (PRESET_EEPROM_ADDRESS + (2*i)), global_presets[i]);
	}	//Save value to eeprom.  Note "update..." not "write..." as eeprom_update_word only writes if the value has changed.
}

//Initialise hardware (AVR peripherals and OLED module).
void hardware_init(void)
{
	clock_prescale_set(clock_div_1);	//needs #include <avr/power.h>, prescaler 1 gives clock speed 8MHz
	usart_init();				//Initialise AVR USART hardware.
	i2c_init();				//Initialise AVR TWI (I2C) hardware.
	oled_init();				//Initialise the OLED module.
	rtc_init();				//Initialise AVR Timer/Counter 2 hardware to operate asynchronously as a real time clock (for accurate timing).

	//Timer/Counter 0 (TC0) is used to effectively turn off the oled screen if no button is pressed for a determined duration.
	TCCR0B |= ((1 << CS02) | (1 << CS00));	//Timer/Counter 0 Conrtol Register B
						//CS0[2:0]: TC0 Closk Select Bits. CS[2:0]=101 correlates to /1024
	TIMSK0 |= (1 << TOIE0);			//TC0 Interrupt Mask
						//TOIE: TC0 Overflow Interrupt Enable.

	//Setup inputs (buttons) and outputs (relay).
	//Note, disabled from hardware initialisation as don't want to interrupt before timers and display are initialised at boot.
	//PCICR |= (1 << BUTTON_PCIE);		//Enable Pin-Change Interrupt for pin-change int pins PCINT[0-7].  This includes all 5 buttons.
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
	//Initialise AVR peripherals and external devices (OLED).
	hardware_init();

	//Test string for USART initialisation.
	usart_print_string("\n\r\ngrind(coffee);\r\n");

	//Test the oled display.
	display_splash_screen();

	//Initialise the presets (pull from eeprom and validate).
	presets_init();

	//Set an initial boot-up timer value (preset A) and display menu and timer on-screen.
	global_sixteenths = global_presets[global_current_preset];
	display_clock();
	display_menu(global_current_preset);

	//Now ready to detect button presses.
	PCICR |= (1 << BUTTON_PCIE);	//Enable pin-change interrupt for buttons.

	while (1)
	{
		//Everything is interrupt driven.
	}

	return 0;	//Never reached.
}
