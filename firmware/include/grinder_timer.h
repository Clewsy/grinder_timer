//#include <avr/power.h>	//To set the clock prescaler in code.


#include "usart.h"
#include "keypad.h"
#include "pwm.h"
#include "timer.h"
//#include "i2c.h"
#include "sh1106.h"


#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0

#define MAX_BRIGHTNESS	100	//8-bit value (0-255) for max LED brightness.

int8_t pulse_dir = -1;

keypad buttons;
usart serial;
pwm led;
timer pulse;
sh1106 oled;