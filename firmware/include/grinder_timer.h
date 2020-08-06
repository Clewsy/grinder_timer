//#include "usart.h"
#include "keypad.h"
#include "pwm.h"
#include "timer.h"
#include "clock.hpp"
#include "sh1106.h"


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