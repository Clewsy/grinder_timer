#include "grinder_timer.h"

//The following interrupt subroutine will be triggered every 1/16th of a second.
//uint8_t global_seconds = 0;	//globally accessible variable used to count seconds.
ISR(TIMER2_OVF_vect)
{
	global_sixteenths--;
	if(global_sixteenths > 60000)		//If global_sixteenths has underflowed (i.e. countdown timer has elapsed).
	{
		RELAY_PORT &= ~(1 << RELAY_PIN);				//Switch the relay back off.
		rtc_disable();							//Disable the timer.
		presets_update_eeprom();					//If the preset was changed, save it to eeprom (after grind to not increase duration).
		_delay_ms(1000);						//Pause with the timer display at zero for one second.
		global_sixteenths = global_presets[global_current_preset];	//Reset the countdown duration to the current preset.
		PCICR |= (1 << BUTTON_PCIE);					//Effectively re-enable the buttons.
	}
	display_clock();	//Refresh the timer display to the value according to the current preset.
}

//The following interrupt sub-routine will be triggered every time there is a change in state of any button.
//ISR has 6 sections, one for each button.
ISR(BUTTON_PCI_VECTOR)
{
	if(global_oled_sleep_flag)			//If the oled is "sleeping" (i.e. turned off the the TC0 overflow interrupt)
	{						//then we just want to turn the screen back on with any button press.
		_delay_ms(BUTTON_DEBOUNCE_DURATION);	//wait for DEBOUNCE_DURATION milliseconds to mitigate effect of switch bounce.
		reset_oled_sleep_timer();		//Call the oled sleep timer reset function.
		while(~BUTTON_PINS & ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT))){}
							//Wait until all buttons are released (avoid running straight through to button-specific action).
		return;					//No other action regardless of button pressed.
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//GRIND BUTTON
	//If statement to capyure actuation of the grind button.
	//This button triggers the relay and begins counting down the timer for the duration in accordance with the selected preset.
	if((BUTTON_PINS & (1 << BUTTON_GRIND)) == 0)	//If the grind button is pressed
	{
		PCICR &= ~(1 << BUTTON_PCIE);	//Effectively disable the buttons while grinding.
		switch(global_current_preset)
		{
			case PRESET_A :
				global_sixteenths = global_presets[PRESET_A];
				break;
			case PRESET_B :
				global_sixteenths = global_presets[PRESET_B];
				break;
			case PRESET_C :
				global_sixteenths = global_presets[PRESET_C];
				break;
			case PRESET_D :
				global_sixteenths = global_presets[PRESET_D];
				break;
		}
		RELAY_PORT |= (1 << RELAY_PIN);
		rtc_enable();
		return;		//The grinder motor should now be running.  Exit from the ISR.
				//Note, the debounce delay is after the grind button if statement so thatthere is
				//no delay when the grind button is pressed.  Debounce only needed for up, down, left and right.
	}

	_delay_ms(BUTTON_DEBOUNCE_DURATION);	//wait for DEBOUNCE_DURATION milliseconds to mitigate effect of switch bounce.

	////////////////////////////////////////////////////////////////////////////////////////
	//LEFT BUTTON
	//If statement to capture actuation of the left button.
	//This button changes the selected preset scrolls backwards through them (A->D->C->B->A)
	if((BUTTON_PINS & (1 << BUTTON_LEFT))  == 0)
	{
		global_current_preset--;			//Decrement the currently selected preset.
		if(global_current_preset == 255)		//Rollover from 0 to 4 (A to D)
		{
			global_current_preset = PRESET_D;
		}
		display_menu(global_current_preset);				//Update the menu to indicate which preset is selected.
		global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
		display_clock();						//Update the timer display to show the preset in seconds.
		presets_update_eeprom();					//If the previous preset was changed, this will save it to eeprom.
		while((BUTTON_PINS & (1 << BUTTON_LEFT))  == 0){}		//Wait until the button is released.
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//RIGHT BUTTON
	//If statement to capture actuation of the right button.
	//This button changes the selected preset scrolls forwards through them (A->B->C->D->A)
	if((BUTTON_PINS & (1 << BUTTON_RIGHT))  == 0)
	{
		global_current_preset++;			//Decrement the currently selected preset.
		if(global_current_preset == 4)			//Rollover from 0 to 4 (A to D)
		{
			global_current_preset = PRESET_A;
		}
		display_menu(global_current_preset);				//Update the menu to indicate which preset is selected.
		global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
		display_clock();						//Update the timer display to show the preset in seconds.
		presets_update_eeprom();					//If the previous preset was changed, this will save it to eeprom.
		while((BUTTON_PINS & (1 << BUTTON_RIGHT))  == 0){}		//Wait until the button is released.
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//UP BUTTON
	//If statement to capture actuation of the up button.
	//This button adjusts upwards the duration of the selected preset.
	if((BUTTON_PINS & (1 << BUTTON_UP)) == 0)
	{
		global_presets[global_current_preset] = (global_presets[global_current_preset] + 4);	//Increment the preset value by 4 sixteenths (0.25s).
		if(global_presets[global_current_preset] > PRESET_MAX)					//Cap the maximum value.
		{
			global_presets[global_current_preset] = PRESET_MAX;
		}
		global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
		display_clock();						//Update the timer display to show the preset in seconds.
		_delay_ms(BUTTON_HOLD_DURATION);				//Implement a delay.  If the button is still held after this:
		while((BUTTON_PINS & (1 << BUTTON_UP)) == 0)			//Run this loop until the button is released.  Fast increment.
		{
			global_presets[global_current_preset] = (global_presets[global_current_preset] + 4);	//Increment the preset value by 4 sixteenths (0.25s).
			if(global_presets[global_current_preset] > PRESET_MAX)					//Cap the maximum value.
			{
				global_presets[global_current_preset] = PRESET_MAX;
			}
			global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
			display_clock();						//Update the timer display to show the preset in seconds.
			_delay_ms(BUTTON_FAST_CHANGE_DURATION);							//Brief delay between increments.
		}
	}

	////////////////////////////////////////////////////////////////////////////////////////
	//DOWN BUTTON
	//If statement to capture actuation of the down button.
	//This button adjusts downwards the duration of the selected preset.
	if((BUTTON_PINS & (1 << BUTTON_DOWN)) == 0)
	{
		global_presets[global_current_preset] = (global_presets[global_current_preset] - 4);	//Deccrement the preset value by 4 sixteenths (0.25s).
		if(global_presets[global_current_preset] > 65000)					//Indicates rollunder from zero.
		{
			global_presets[global_current_preset] = 0;
		}
		global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
		display_clock();						//Update the timer display to show the preset in seconds.
		_delay_ms(BUTTON_HOLD_DURATION);				//Implement a delay.  If the button is still held after this:
		while((BUTTON_PINS & (1 << BUTTON_DOWN)) == 0)			//Run this loop until the button is released.  Fast increment.
		{
			global_presets[global_current_preset] = (global_presets[global_current_preset] - 4);	//Decrement the preset value by 4 sixteenths (0.25s).
			if(global_presets[global_current_preset] > 65000)					//Indicates rollunder from zero.
			{
				global_presets[global_current_preset] = 0;
			}
			global_sixteenths = global_presets[global_current_preset];	//Set the current preset to the timer in prep for grinding
			display_clock();						//Update the timer display to show the preset in seconds.
			_delay_ms(BUTTON_FAST_CHANGE_DURATION);							//Brief delay between increments.
		}
	}
}//Interrupt flag PCIF0 (pin-change interrupt flag 0) on PCIFR (pin-change interrupt flag register) is automatically cleared upon exit of this ISR.

//Timer/Counter 0 overflow interrupt vector.
//TC0 is used as a simple counter prescaled by 1024 (~7812Hz) that triggers this interrupt at overflow.
//The interrupt function just increments a counter but when the counter reaches a nominated value,
//the oled is put to "sleep" (screen turned off).
ISR(TIMER0_OVF_vect)
{
	global_oled_sleep_timer++;			//Increment the oled sleep timer counter.
	if(global_oled_sleep_timer > OLED_SLEEP_TIMER)	//If the counter reaches a determined value.
	{
		global_oled_sleep_timer = 0;		//Reset the counter.
		global_oled_sleep_flag = OLED_SLEEP_ON;	//Flag that the oled is "sleeping".
		oled_send_command(SET_DISPLAY_OFF);	//Turn off the oled.
	}
}

//This function resets the counter that would otherwise eventually put the oled to "sleep".
//Whenever called, this function will effectively reset a countdown (countup) to turning off the oled.
void reset_oled_sleep_timer(void)
{
	oled_send_command(SET_DISPLAY_ON);		//Ensure the oled is turned on
	global_oled_sleep_flag = OLED_SLEEP_OFF;	//Flag that the oled is not "sleeping".
	global_oled_sleep_timer = 0;			//Reset the oled sleep timer counter.
}

//This function displays the current value of the timer by converting the quantity of "global_sixteenths" to seconds.
////The digits are displayed with the large seven-segment style font.
void display_clock(void)
{
	oled_type_digit_large(((global_sixteenths >> 4) / 10), 4, 2);			//Tens of seconds. RShift 4 bits to divide by 16. Print with top left at column 4, page 2.
	oled_type_digit_large(((global_sixteenths >> 4) % 10), 28, 2);			//Ones of seconds. % (i.e. mod) gives remainder after dividing.
	oled_type_digit_large(11, 52, 2);						//The eleventh character in this font is a period ('.').  This serves as a separator.
	oled_type_digit_large(((uint16_t)(global_sixteenths*0.625) % 10), 76, 2);	//Tenths of seconds. Note division and modulus operators ignore fractions.
	oled_type_digit_large(((uint16_t)(global_sixteenths*6.25) % 10) , 100, 2);	//Hudredths of a second. Note modulus operation (%) only works on integers.
}

//Draws the preset menu at the top of the screen.  Presets represented by A, B, C and D.
//The currently selected preset is indicated by a box around the letter.
void display_menu(uint8_t selected_preset)
{
	uint8_t i;
	for(i=0;i<4;i++)	//Repeat four times for presets A, B, C and D.
	{
		//If statement will usually just draw the characters A to D, except in the case of the
		//selected_preset character.  By adding 4 to the font map, the alternate character Will
		//be selected (same character but within a box)
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
	oled_clear_screen();
	oled_set_address(22,3);
	oled_type_string("grind(coffee);");
	_delay_ms(1000);
	oled_clear_screen();
}

//This function is run on start-up to pull the presets saved to EEPROM and make them easily, globally accessible.
//It will also set the presets to zero if they are not a valid number (such as after flashing a new AVR).
//To be valid, the preset must be a multiple of 4 (only possible increments/decrements of sixteenths)
//and also less than PRESET_MAX.
void presets_init(void)
{
	uint8_t i;
	for(i=0;i<4;i++)	//Run through for each of the four presets.
	{
		global_presets[i] = eeprom_read_word((uint16_t *) (PRESET_EEPROM_ADDRESS + (2*i)));	//Read the current preset from eeprom
		if ((global_presets[i] > PRESET_MAX) || (global_presets[i] % 4))			//Check if it's not valid
		{
			global_presets[i] = 0;								//Reset preset to 0
			eeprom_update_word((uint16_t *) (PRESET_EEPROM_ADDRESS + (2*i)), 0);
		}	//Save valid value (0) to eeprom.
	}
}

//This function compares the presets in use to those stored in eeprom.  If they differ then the eeprom is updated.
//It will be called when changing betwwen presets or starting the grinder as these are the times the presets have possibly changed.
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

	//Timer/Counter o (TC0) is used to effectively turn off the oled screen if no button is pressed for a determined duration.
	TCCR0B |= ((1 << CS02) | (1 << CS00));	//Timer/Counter 0 Conrtol Register B
						//CS0[2:0]: TC0 Closk Select Bits. CS[2:0]=101 correlates to /1024
	TIMSK0 |= (1 << TOIE0);			//TC0 Interrupt Mask
						//TOIE: TC0 Overflow Interrupt Enable.

	//Setup inputs (buttons) and outputs (relay).
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
	//Initialise AVR peripherals and external devices (oled).
	hardware_init();

	//Test string for USART initialisation
	usart_print_string("\n\r\ngrind(coffee);\r\n");

	//Test the oled display
	display_splash_screen();

	//Set an initial boot-up timer value and display menu and tiomer on-screen.
	presets_init();
	global_sixteenths = global_presets[global_current_preset];
	display_clock();
	display_menu(global_current_preset);

	while (1)
	{
		//Everything is interrupt driven.
	}

	return 0;	//Never reached.
}
