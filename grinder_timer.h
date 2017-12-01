#include <avr/io.h>		//Defines standard pins, ports, etc
#include <util/delay.h>		//Needed to call _delay_ms(xxxx)
#include <avr/power.h>		//Added to set clock prescaler in code

#include "usart.h"		//Module to utilise AVR serial communications via the onboard USART hardware.  Mostly used for debugging by outputting data over USB.
#include "i2c.h"		//Module to utilise the AVRs onboard I2C hardware (which Atmel calls TWI).  Required for communicating with the oled module.
#include "ssd1306.h"		//Module for controling the oled display.
#include "rtc.h"		//Module for utilising timer/counter 2 as a real-time-clock (externally connected 32.768kHz crystal).

#define RELAY_PORT	PORTC	//Port to which the relay is connected.
#define RELAY_DDR	DDRC	//Data-direction register to set as an output the pin to which the relay is connected.
#define RELAY_PIN	PC0	//The specific pin to which the relay is connected.

//Button pin/port/register definitions.  Note, all buttons must be on the same port (B,C or D)
#define BUTTON_PORT			PORTD		//Port to which the buttons are connected.
#define BUTTON_PINS			PIND		//Register for reading pins status for the buttons.
#define BUTTON_DDR			DDRD		//Data-direction register to set as inputs the pins to which the buttons are connected.
#define BUTTON_PCIE			PCIE2		//Pin-change interrupt enable used to enable the pin-change interrupt to be triggered by any of the buttons.
#define BUTTON_PCMSK			PCMSK2		//Pin change mask register to enable pin-change interrupts on the connected buttons.
#define BUTTON_UP			PD3		//The specific pin to which the up button is connected.
#define BUTTON_DOWN			PD4		//The specific pin to which the down button is connected.
#define BUTTON_LEFT			PD5		//The specific pin to which the left button is connected.
#define BUTTON_RIGHT			PD6		//The specific pin to which the right button is connected.
#define BUTTON_GRIND			PD7		//The specific pin to which the grind button is connected.
#define BUTTON_PCI_VECTOR		PCINT2_vect	//The ISR (interrupt sub-routine) vector for the function that is triggered upon any pin-change (button press).
#define BUTTON_DEBOUNCE_DURATION	20		//The duration in milliseconds used for the button debounce.
#define BUTTON_HOLD_DURATION		1000		//The button hold duration after which a fast increment/decrement will begin
#define BUTTON_FAST_CHANGE_DURATION	10		//The duration between fast increments/decrements.

#define PRESET_A	0	//Value to indicate the currently selected preset is preset 'A'
#define PRESET_B	1	//Value to indicate the currently selected preset is preset 'B'
#define PRESET_C	2	//Value to indicate the currently selected preset is preset 'C'
#define PRESET_D	3	//Value to indicate the currently selected preset is preset 'D'
#define PRESET_MAX	960	//The maximum possible 16ths for the countdown duration (equates to 60 seconds)

//Global variables defined here.
uint16_t global_sixteenths = 0;			//globally accessible variable used to count sixteenths of a second.i
uint16_t global_presets[] = {192,160,128,96};	//globally accessible array - each cell is the value of a preset timer duration (in sixteenths)
uint8_t global_current_preset = PRESET_A;	//globally accessible variable for determining the currently selected preset

//Function declarations
void oled_display_clock(void);
void display_clock(void);
void hardware_init(void);
void display_menu(uint8_t selected_preset);
