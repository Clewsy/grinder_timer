// Header file to define a generic hardware timer that triggers an interrupt sub-routine.

#include <avr/io.h>
#include <avr/interrupt.h>	// For using interrupt sub-routine vectors.

#define SLEEPER_TCCRA		TCCR0A		// Timer/Counter Control Register A
#define SLEEPER_TCCRB		TCCR0B		// Timer/Counter Control Register B
#define SLEEPER_CS2		CS02		// Timer/Counter Clock Select Bit 2
#define SLEEPER_CS1		CS01		// Timer/Counter Clock Select Bit 1
#define SLEEPER_CS0		CS00		// Timer/Counter Clock Select Bit 0
#define SLEEPER_TCNT		TCNT0		// Timer/Counter Value Register.
#define SLEEPER_TIMSK		TIMSK0		// Timer/Counter Timer Interrupt Mask Register
#define SLEEPER_IE		TOIE0		// Timer Overflow Interrupt Enable Bit.
#define SLEEPER_INT_VECTOR	TIMER0_OVF_vect	// Interrupt subroutine name.

class sleeper
{
	public:
		void init(void);
		void enable(bool enable);
		uint16_t counter = 0;
		bool sleeping = false;
};
