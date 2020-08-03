
#include "usart.h"
#include "keypad.h"
#include "pwm.h"

#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0


keypad buttons;
usart serial;
pwm led;