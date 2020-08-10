#include "timer.hpp"

void timer::init(void)
{
	// WGM[3:0] set to 0100 : CTC mode, counts from 0 to value of output compare register.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 001 : clk/1 (no prescaling).
	TIMER_TCCRB |= ((1 << TIMER_WGM2) | (1 << TIMER_CS1));

}

void timer::enable(bool enable)
{
	if(enable)	TIMER_TIMSK |= (1 << TIMER_IE);		// Enable the output compare interrupt.
	else		TIMER_TIMSK &= ~(1 << TIMER_IE);	// Disable the output compare interrupt.
}

void timer::set(uint16_t value)
{
	// Set the value of the output compare register (16-bit register).
	// Use this value to determine the pulse "speed".
	TIMER_SET_REG = value;
}