#include "pulser.hpp"

void pulser::init(void)
{
	// WGM[3:0] set to 0100 : CTC mode, counts from 0 to value of output compare register.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 001 : clk/1 (no prescaling).
	PULSER_TCCRB |= ((1 << PULSER_WGM2) | (1 << PULSER_CS1));

}

void pulser::enable(bool enable)
{
	if(enable)	PULSER_TIMSK |= (1 << PULSER_IE);	// Enable the output compare interrupt.
	else		PULSER_TIMSK &= ~(1 << PULSER_IE);	// Disable the output compare interrupt.
}

void pulser::set(uint16_t value)
{
	// Set the value of the output compare register (16-bit register).
	// Use this value to determine the pulse "speed".
	PULSER_SET_REG = value;
}