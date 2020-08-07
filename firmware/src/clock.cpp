#include "clock.hpp"

void clock::init(void)
{
	// Ensure all of the timer/counter interrupts are disabled during initialisation.
	CLOCK_TIMSK &= ~((1 << CLOCK_OCIEA) | (1 << CLOCK_OCIEB) | (1 << CLOCK_TOIE));

	// Set AS bit so timer/counter is clocked from external crystal connected to TOSC1 pin.
	CLOCK_ASSR |= (1 << CLOCK_AS);

	// Reset the current value of the timer/counter.
	CLOCK_TCNT = 0;

	// Use Clock Select Bits in the TC control register B (TCCRB) to configure the timer/counter overflow period.
	// I.e. the frequency of triggering the timer/counter overflow interrupt sub-routine.
	//////////////////////////////////////////////////////
	// | CS2 | CS1 | CS0 | Description | Overflow Period |
	// |  0  |  0  |  0  | T/C Stopped | -               |
	// |  0  |  0  |  1  | clkT0S      | 1/128s          |
	// |  0  |  1  |  0  | clkT0S/8    | 1/16s           |
	// |  0  |  1  |  1  | clkT0S/32   | 1/4s            |
	// |  1  |  0  |  0  | clkT0S/64   | 1/2s            |
	// |  1  |  0  |  1  | clkT0S/128  | 1s              |
	// |  1  |  1  |  0  | clkT0S/256  | 2s              |
	// |  1  |  1  |  1  | clkT0S/1024 | 8s              |
	//////////////////////////////////////////////////////
//	CLOCK_TCCRB |= (1 << CLOCK_CS0);			// Configured to trigger ISR every 1/128 seconds.
	CLOCK_TCCRB |= (1 << CLOCK_CS1);			// Configured to trigger ISR every 1/16 seconds.
//	CLOCK_TCCRB |= ((1 << CLOCK_CS2) | (1 << CLOCK_CS0));	// Configured to trigger ISR every 1 second.

	// Wait until all initialisation register writes are complete.
	// All "update busy"bits in the ASSR register should return to 0.
	while(CLOCK_ASSR & ((1 << CLOCK_TCUB) | (1 << CLOCK_OCRAUB) | (1 << CLOCK_OCRBUB) | (1 << CLOCK_TCRAUB) | (1 << CLOCK_TCRBUB))) {}
}

void clock::enable(void)
{
	CLOCK_TIMSK |= (1 << CLOCK_TOIE);
}

void clock::disable(void)
{
	CLOCK_TIMSK &= ~(1 << CLOCK_TOIE);

}