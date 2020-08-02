//Header file for pwm output.

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>	//For using inetrrupt registers and sub-routine vectors.
#include <util/delay.h>		//Needed to use _delau_ms() for de-bouncing buttons.


#define LED_PORT	PORTD
#define LED_DDR		DDRD
#define LED_PIN		PD2

#define LED_TCCR	TCCR3A	//Timer/Counter Control Register






class pwm
{
	public:
		void init(void);
//		void enable(void);
//		void disable(void);
//		bool state(uint8_t button);
};

