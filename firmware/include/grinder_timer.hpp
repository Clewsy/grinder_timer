//#include "usart.hpp"
#include "keypad.hpp"
#include "pwm.hpp"
#include "timer.hpp"
#include "clock.hpp"
#include "sh1106.hpp"

// Button de-bounce duration in milliseconds.
#define BUTTON_DEBOUNCE_MS	10

#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0

// 8-bit value (0-255) for max LED brightness.
#define MAX_BRIGHTNESS	100

// The value of pulse_dir switches between 1 and -1.
// Enables tracking of the LED pulse effect, i.e. 1:getting brighter, 2:getting dimmer.
int8_t pulse_dir = -1;

// Initialise the various classes used.
//usart serial;	// Serial interface - really only used for debugging.
keypad buttons;	// 5-button keypad.
pwm led;	// LED is connected to a pwm output to enable variable brightness.
timer pulse;	// A timer is used to create a pulsing effectwith the LED. 
clock rtc;	// A real-time clock (using an external 32.768kHz crystal) is used for acurate timing.
sh1106 oled;	// A 128x64 pixel oled with a sh1106 driver.

// When the rtc timer is running, "counter" is incremented by the timer overflow ISR.
// This timer is configured such that every increment represents a sixteenth of a second.
uint16_t counter = 0;
