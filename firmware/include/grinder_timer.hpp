//#include "usart.hpp"
#include "keypad.hpp"
#include "pwm.hpp"
#include "timer.hpp"
#include "clock.hpp"
#include "sh1106.hpp"

// Button de-bounce duration in milliseconds.
#define BUTTON_DEBOUNCE_MS	10

// Timer presets.
#define PRESET_A	0
#define PRESET_B	1
#define PRESET_C	2
#define PRESET_D	3

#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0
#define RELAY_ON	RELAY_PORT |= (1 << RELAY_PIN)
#define RELAY_OFF	RELAY_PORT &= ~(1 << RELAY_PIN)

// Definitions used by the led_control() function.
#define LED_OFF			00
#define LED_ON			01
#define LED_PULSE		10
#define LED_PULSE_SPEED		8000
#define LED_MAX_BRIGHTNESS	150

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
uint16_t counter = 1280;

// This variable contains a numerical value corresponding to the currently selected timer preset - A, B, C or D.
uint8_t current_preset = PRESET_A;

// This array contains the preset timer values (as multiples of sixteenths of a secons).
uint16_t preset_timer[4] = {160, 320, 640, 1280};

// A flag that indicates whether the grinder motor is currently running.
bool grinding = false;



// Function declarations.
ISR(BUTTON_PCI_VECTOR);		// Interrupt subroutine triggered by a button press.
void led_control(uint8_t led_mode);
void grind(bool grind);
void handle_up_down(uint8_t left_or_right);
void handle_left_right(uint8_t left_or_right);
ISR(TIMER_INT_VECTOR);		// Interrupt subroutine triggered by the LED pulse effect timer.
ISR(CLOCK_INT_VECTOR);		// Interrupt subroutine triggered by the real-time clock tiomer.
void refresh_timer(void);	// 
void refresh_menu(void);	// Update the preset selection icons to show which preset is currently selected.
void splash(void);		// Silly animation that runs at poer on.
void hardware_init();		// Initialise peripherals.
int main(void);			// contains the infinite loop.
