//#include "usart.hpp"
#include "keypad.hpp"
#include "pwm.hpp"
#include "timer.hpp"
#include "clock.hpp"
#include "sh1106.hpp"

// Button definitions.
#define BUTTON_DEBOUNCE_MS	10	// Button de-bounce duration in milliseconds.
#define BUTTON_LONG_PRESS	500	// Duration (in ms approximately) to wait before registering a long-press.
#define BUTTON_FAST_CHANGE	50	// Duration (in ms approximately) between actions when the button is held in. (long-press).

// Timer preset definitions.
#define PRESET_A		0
#define PRESET_B		1
#define PRESET_C		2
#define PRESET_D		3
#define PRESET_MIN_VALUE	16	// Minimum value that can be used for a preset (in sixteenths of a second).
#define PRESET_MAX_VALUE	960	// Maximum value that can be used for a preset (in sixteenths of a second).

// Relay control definitions.
#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0
#define RELAY_ON	RELAY_PORT |= (1 << RELAY_PIN)	// Macro to turn the relay (and therefore the grinder) on.
#define RELAY_OFF	RELAY_PORT &= ~(1 << RELAY_PIN)	// Macro to turn the relay (and therefore the grinder) off.

// Definitions used by the led_control() function.
#define LED_OFF			00
#define LED_ON			01
#define LED_PULSE		10
#define LED_PULSE_SPEED		8000
#define LED_MAX_BRIGHTNESS	150

// The value of pulse_dir switches between 1 and -1.
// Enables tracking of the LED pulse effect, i.e. 1:getting brighter, 2:getting dimmer.
int8_t pulse_dir = -1;

// This variable contains a numerical value corresponding to the currently selected timer preset - A, B, C or D.
uint8_t current_preset = PRESET_A;

// This array contains the preset timer values (as multiples of sixteenths of a seconds).
uint16_t preset_timer[4] = {144, 192, 24, 72};

// When the rtc timer is running, "counter" is incremented by the timer overflow ISR.
// This timer is configured such that every increment represents a sixteenth of a second.
uint16_t counter = preset_timer[PRESET_A];

// A flag that indicates whether the grinder motor is currently running.
bool grinding = false;

// Initialise the various classes used.
//usart serial;	// Serial interface - really only used for debugging.
keypad buttons;	// 5-button keypad.
pwm led;	// LED is connected to a pwm output to enable variable brightness.
timer pulse;	// A timer is used to create a pulsing effectwith the LED. 
clock rtc;	// A real-time clock (using an external 32.768kHz crystal) is used for acurate timing.
sh1106 oled;	// A 128x64 pixel oled with a sh1106 driver.

// Function declarations.
ISR(BUTTON_PCI_VECTOR);				// Interrupt subroutine triggered by a button press.
ISR(TIMER_INT_VECTOR);				// Interrupt subroutine triggered by the LED pulse effect timer.
ISR(CLOCK_INT_VECTOR);				// Interrupt subroutine triggered by the real-time clock tiomer.
void led_control(uint8_t led_mode);		// Set the led to either off, on or pulsing.
void handle_up_down(uint8_t left_or_right);	// Increase or decrease the value of the current selected preset.
void handle_left_right(uint8_t left_or_right);	// Scroll left or right through the different presets.
void grind(bool grind);				// Start or stop the grinder.
void refresh_timer(void);			// Update the countdown timer section of the oled.
void refresh_menu(void);			// Update the preset selection icons to show which preset is currently selected.
void splash(void);				// Silly animation that runs at poer on.
void hardware_init();				// Initialise peripherals.
int main(void);					// contains the infinite loop.
