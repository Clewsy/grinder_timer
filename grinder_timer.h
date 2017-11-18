#include <avr/io.h>		//Defines standard pins, ports, etc
#include <util/delay.h>		//Needed to call _delay_ms(xxxx)
#include <avr/power.h>		//Added to set clock prescaler in code

#include "usart.h"//Module to utilise AVR serial communications via the onboard USART hardware.  Mostly used for debugging by outputting data over USB.
#include "i2c.h"//Module to utilise the AVRs onboard I2C hardware (which Atmel calls TWI).  Required for communicating with the oled module.
#include "ssd1306.h"//Module for controling the oled display.
#include "rtc.h"//Module for utilising timer/counter 2 as a real-time-clock (externally connected 32.768kHz crystal).

#define RELAY_PORT		PORTC	//Port to which the relay is connected.
#define RELAY_DDR		DDRC	//Data-direction register to set as an output the pin to which the relay is connected.
#define RELAY_PIN		PC0	//The specific pin to which the relay is connected.

#define BUTTON_PORT		PORTB	//Port to which the buttons are connected.
#define BUTTON_PINS		PINB	//Register for reading pins status for the buttons.
#define BUTTON_DDR		DDRB	//Data-direction register to set as inputs the pins to which the buttons are connected.
#define BUTTON_GRIND		PB0	//The specific pin to which the grind button is connected.

//Global variables defined here.
uint16_t global_sixteenths = 0;         //globally accessible variable used to count sixteenths of a second.i
uint16_t global_timer_setpoint = 192;   //globally accessible variable used to control the timer countdown duration.

//Function declarations
void oled_display_clock(void);
void display_clock(void);
void hardware_init(void);
