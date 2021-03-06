#include <avr/io.h>		//Defines standard pins, ports, etc
#include <util/delay.h>		//Needed to call _delay_ms(xxxx)
#include <avr/power.h>		//Added to set clock prescaler in code
#include <avr/eeprom.h>		//Added to allow saving presets to eeprom (to survive powercycle).

#include "usart.h"		//Functions to utilise AVR serial communications via the onboard USART hardware.  Used for debugging by outputting over USB.
#include "i2c.h"		//Functions to utilise the AVRs onboard I2C hardware (which Atmel calls TWI).  Required for communicating with the OLED module.
#include "ssd1306.h"		//Functions for controling the OLED display.
#include "rtc.h"		//Functions for utilising timer/counter 2 as a real-time-clock (externally connected 32.768kHz crystal).

//OLED sleep definitions (not true sleep, just turns the display on/off)
#define OLED_SLEEP_OFF		0	//Flag value to indicate the OLED is not sleeping (display on)
#define OLED_SLEEP_ON		1	//Flag value to indicate the OLED is sleeping (display off)
#define OLED_SLEEP_TIMER	3662	//This value is used to determine the duration after which the OLED display is switched off.
					//This function uses Timer/Counter 0 in overflow mode with prescaler set to F_CPU/1024.
					//Therefore, the counter overflows at 256/(8000000/1024)Hz = 0.032768Hz.  Every overflow increments
					//a 16-bit counter (global_sleep_timer).  When this counter reaches OLED_SLEEP_TIMER, the "sleep"
					//function is run (not using AVR's sleep functionality, just turning of the OLED).
					//OLED_SLEEP_TIMER = 305 equates to about ten seconds (0.032768*305 = 9.99424s).
					//OLED_SLEEP_TIMER = 915 equates to about thirty seconds (0.032768*915 = 29.98272s).
					//OLED_SLEEP_TIMER = 3662 equates to about two minutes (0.032768*3662 = 119.996416s).

//Input definitions
//Button pin/port/register definitions.  Note, all buttons must be on the same port (B,C or D)
#define BUTTON_PORT			PORTD		//Port to which the buttons are connected.
#define BUTTON_PINS			PIND		//Register for reading pins status for the buttons.
#define BUTTON_DDR			DDRD		//Data-direction register for setting as inputs the pins to which the buttons are connected.
#define BUTTON_PCIE			PCIE2		//Pin-change interrupt enable used to enable the pin-change interrupt to be triggered by the buttons.
#define BUTTON_PCMSK			PCMSK2		//Pin change mask register to enable pin-change interrupts on the connected buttons.
#define BUTTON_UP			PD3		//The specific pin to which the up button is connected.
#define BUTTON_DOWN			PD4		//The specific pin to which the down button is connected.
#define BUTTON_LEFT			PD5		//The specific pin to which the left button is connected.
#define BUTTON_RIGHT			PD6		//The specific pin to which the right button is connected.
#define BUTTON_GRIND			PD7		//The specific pin to which the grind button is connected.
#define BUTTON_PCI_VECTOR		PCINT2_vect	//The interrupt sub-routine vector (function) that is triggered upon any pin-change (button press).
#define BUTTON_DEBOUNCE_DURATION	10		//The duration in milliseconds used for the button debounce.
#define BUTTON_HOLD_DURATION		1000		//The button hold duration in milliseconds after which a fast increment/decrement will begin.
#define BUTTON_FAST_CHANGE_DURATION	10		//The duration between fast increments/decrements (in milliseconds).

//Output definitions
#define RELAY_PORT	PORTC	//Port to which the relay is connected.
#define RELAY_DDR	DDRC	//Data-direction register to set as an output the pin to which the relay is connected.
#define RELAY_PIN	PC0	//The specific pin to which the relay is connected.

//Preset definitions
#define PRESET_A		0	//Value to indicate the currently selected preset is preset 'A'
#define PRESET_B		1	//Value to indicate the currently selected preset is preset 'B'
#define PRESET_C		2	//Value to indicate the currently selected preset is preset 'C'
#define PRESET_D		3	//Value to indicate the currently selected preset is preset 'D'
#define PRESET_MAX		960	//The maximum possible 16ths for the countdown duration (960 equates to 60 seconds)
#define PRESET_EEPROM_ADDRESS	0	//Address in eeprom at which the presets are saved (4x16-bit words)

//Global variables defined here.
uint16_t global_sixteenths = 0;				//Globally accessible variable used to count sixteenths of a second.
uint16_t global_presets[4];				//Globally accessible array - each cell is the value of a preset timer duration (in sixteenths).
uint8_t global_current_preset = PRESET_A;		//Globally accessible variable for determining the currently selected preset.
uint16_t global_oled_sleep_timer = 0;			//Globally accessible variable for counting TC0 overflows which controls the "sleep" mode.
uint8_t global_oled_sleep_flag = OLED_SLEEP_OFF;	//Flag to determine if the screen is "sleeping".

//Function declarations
ISR(TIMER2_OVF_vect);
ISR(BUTTON_PCI_VECTOR);
ISR(TIMER0_OVF_vect);
void increment_current_preset(void);
void decrement_current_preset(void);
void reset_oled_sleep_timer(void);
void display_clock(void);
void display_menu(uint8_t selected_preset);
void display_splash_screen(void);
void presets_init(void);
void presets_update_eeprom(void);
void hardware_init(void);
