#include "timer.hpp"

void timer::init(void)
{
	// WGM[3:0] set to 0100 : CTC mode, counts from 0 to value of output compare register.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 001 : clk/1 (no prescaling).
	TIMER_TCCRB |= ((1 << TIMER_WGM2) | (1 << TIMER_CS1));

	// Set the value of the output compare register (16-bit register).
	// Use this value to determine the pulse "speed".
	TIMER_SET_REG = TIMER_SET_VAL;
}

void timer::enable(void)
{
	// Enable the output compare interrupt.
	TIMER_TIMSK |= (1 << TIMER_IE);
}

void timer::disable(void)
{
	// Disable the output compare interrupt.
	TIMER_TIMSK &= ~(1 << TIMER_IE);
}
