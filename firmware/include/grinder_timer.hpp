//#include "usart.hpp"
#include "keypad.hpp"
#include "pwm.hpp"
#include "pulser.hpp"
#include "clock.hpp"
#include "sleeper.hpp"
#include "sh1106.hpp"

// Button definitions.
#define BUTTON_DEBOUNCE_MS	10	// Button de-bounce duration in milliseconds.
#define BUTTON_LONG_PRESS	500	// Duration (in ms approximately) to wait before registering a long-press.
#define BUTTON_FAST_CHANGE	50	// Duration (in ms approximately) between actions when the button is held in. (long-press)
#define UP			1	// Value indicates up button - used in handle_up_or_down() and change_preset_value() functions.
#define DOWN			-1	// Value indicates down button - used in handle_up_or_down() and change_preset_value() functions.
#define LEFT			-1	// Value indicates left button - used in handle_left_or_right() function.
#define RIGHT			1	// Value indicates right button - used in handle_left_or_right() function.

// Timer preset definitions.
#define PRESET_A		0
#define PRESET_B		1
#define PRESET_C		2
#define PRESET_D		3
#define PRESET_MIN_VALUE	16	// Minimum value that can be used for a preset (in sixteenths of a second).
#define PRESET_MAX_VALUE	960	// Maximum value that can be used for a preset (in sixteenths of a second).

// Relay control definitions.
#define RELAY_PORT		PORTC				// Port containing the pin connected to the relay.
#define RELAY_DDR		DDRC				// Data direction register for the port containing the pin connected to the relay.
#define RELAY_PIN		PC0				// Pin connected to the relay coil.
#define RELAY_ON		RELAY_PORT |= (1 << RELAY_PIN)	// Macro to turn the relay (and therefore the grinder) on.
#define RELAY_OFF		RELAY_PORT &= ~(1 << RELAY_PIN)	// Macro to turn the relay (and therefore the grinder) off.
#define RELAY_RESET_DELAY	1000				// Delay in milliseconds after grinding has finished before resetting the timer.

// Definitions used by the led_control() function.
#define LED_OFF			00	// Set LED to off.
#define LED_ON			01	// Set LED to on.
#define LED_PULSE		11	// Set LED to pulsing.
#define LED_PULSE_SPEED		8000	// Sets the pulse speed.
#define LED_MAX_BRIGHTNESS	150	// Sets the max brightness of the LED when pulsing or on (pwm pulse-width 0 to 255).

// Definition for defining the duration after which the unit will enter a "sleep" mode.
#define SLEEP_COUNTER	305
// The sleeper timer/counter is configured such that the frequency of this interrupt (when enabled) is 0.032768Hz.
// Delay(s) = sleep_counter * 0.032768Hz
// Therefore, the various delays require count values such as:
//   10s : sleep_counter ~ 305
//   30s : sleep_counter ~ 916
//   60s : sleep_counter ~ 1831
//  120s : sleep_counter ~ 3662

////////////////////////////////////////
// Global variable declarations.

// This variable contains a numerical value corresponding to the currently selected timer preset - A, B, C or D.
uint8_t current_preset = PRESET_A;

// This array contains the preset timer values (as multiples of sixteenths of a seconds).
uint16_t preset_timer[4] = {144, 192, 24, 72};

// A flag that indicates whether the grinder motor is currently running.
bool grinding = false;

// Initialise the various classes used.
//usart serial;	// Serial interface - really only used for debugging.
keypad buttons;	// 5-button keypad.
sleeper sleep;	// (Uses Timer/Counter 0) Trigger an interrupt after a set duration to enter a "sleep" mode.
pulser pulse;	// (Uses Timer/Counter 1) A timer is used to create a pulsing effect with the LED. 
pwm led;	// (Uses Timer/Counter 3) LED is connected to a pwm output to enable variable brightness.
clock rtc;	// (Uses Timer/Counter 2) A real-time clock (using an external 32.768kHz crystal) is used for acurate timing.
sh1106 oled;	// A 128x64 pixel oled with a sh1106 driver.

////////////////////////////////////////
// Function declarations.

ISR(SLEEPER_INT_VECTOR);
ISR(BUTTON_PCI_VECTOR);				// Interrupt subroutine triggered by a button press.
ISR(TIMER_INT_VECTOR);				// Interrupt subroutine triggered by the LED pulse effect timer.
ISR(CLOCK_INT_VECTOR);				// Interrupt subroutine triggered by the real-time clock tiomer.
void led_control(uint8_t led_mode);		// Set the led to either off, on or pulsing.
void change_preset_value(int8_t up_or_down);	// Increase or decrease the value of the current selected preset.
void handle_up_down(int8_t up_or_down);		// Handle pressing or holding the up or down buttons.
void handle_left_right(int8_t left_or_right);	// Scroll left or right through the different presets.
void grind(bool grind);				// Start or stop the grinder.
void refresh_timer(void);			// Update the countdown timer section of the oled.
void refresh_menu(void);			// Update the preset selection icons to show which preset is currently selected.
void splash(void);				// Silly animation that runs at poer on.
void hardware_init(void);			// Initialise peripherals.
int main(void);					// contains the infinite loop.
