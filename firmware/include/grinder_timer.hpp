//#include "usart.h"
#include "keypad.hpp"
#include "pwm.hpp"
#include "timer.hpp"
#include "clock.hpp"
#include "sh1106.hpp"

#define BUTTON_DEBOUNCE_MS	10	// Button de-bounce duration in milliseconds.


#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0

#define MAX_BRIGHTNESS	100	//8-bit value (0-255) for max LED brightness.

int8_t pulse_dir = -1;

keypad buttons;
//usart serial;
pwm led;
timer pulse;
clock rtc;
sh1106 oled;