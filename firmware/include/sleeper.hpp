// Header file to define a generic hardware timer that triggers an interrupt sub-routine.

#include <avr/io.h>
#include <avr/interrupt.h>	// For using interrupt sub-routine vectors.

// Definitions for configuring the timer.
#define SLEEPER_TCCRA		TCCR0A		// Timer/Counter Control Register A
#define SLEEPER_TCCRB		TCCR0B		// Timer/Counter Control Register B
#define SLEEPER_CS2		CS02		// Timer/Counter Clock Select Bit 2
#define SLEEPER_CS1		CS01		// Timer/Counter Clock Select Bit 1
#define SLEEPER_CS0		CS00		// Timer/Counter Clock Select Bit 0
#define SLEEPER_TCNT		TCNT0		// Timer/Counter Value Register.
#define SLEEPER_TIMSK		TIMSK0		// Timer/Counter Timer Interrupt Mask Register
#define SLEEPER_IE		TOIE0		// Timer Overflow Interrupt Enable Bit.
#define SLEEPER_INT_VECTOR	TIMER0_OVF_vect	// Interrupt subroutine name.

// The duration after which the unit will enter a "sleep" mode.
#define SLEEP_COUNTER		1831
// The sleeper timer/counter is configured such that the frequency of this interrupt (when enabled) is 0.032768Hz.
// Delay(s) = sleep_counter * 0.032768Hz
// Therefore, the various delays require count values such as:
//   10s : sleep_counter ~ 305
//   30s : sleep_counter ~ 916
//   60s : sleep_counter ~ 1831
//  120s : sleep_counter ~ 3662

class sleeper
{
	public:
		void init(void);
		void enable(bool enable);
		void reset(void);		// Reset counter to 0.  I.e. effectively restart the timer.
		uint16_t counter = 0;		// Counter can be used to tract number of times the ISR has been triggered.
		bool sleeping = false;		// A flag for tracking the current sleep state.
};
