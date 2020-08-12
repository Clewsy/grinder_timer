#include "sleeper.hpp"

void sleeper::init(void)
{
	// WGM[3:0] set to 0000 : Normal mode, counts from 0 to 0xFF then overflows.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 101 : clk/1024.  I.e. counter increments at a frequency of F_CPU/1024.
	SLEEPER_TCCRB |= ((1 << SLEEPER_CS2) | (1 << SLEEPER_CS0));

	// This is an 8-bit timer/counter.  Therefore, configured as above with prescaler = ckl/1024,
	// the counter will increment with frequency:
	// f = Fcpu/prescaler 
	// Where	Fcpu = 8,000,000Hz (internal oscillator)
	//		prescaler = 1024 (set with CS bts)
	// F = 8000000/1024 = 7812.5Hz.
	// This is the frequency at which the counter increments.  Therefore the frequency of the overflow interrupt Fov:
	// Fov = 256 / 7812.5 ~ 0.032768Hz.
}

// Enable (true) or disable (false) the sleeper timer.
void sleeper::enable(bool enable)
{
	if(enable)	SLEEPER_TIMSK |= (1 << SLEEPER_IE);	// Enable the output compare interrupt.
	else		SLEEPER_TIMSK &= ~(1 << SLEEPER_IE);	// Disable the output compare interrupt.
}

// Reset the sleeper counter.
void sleeper::reset(void)
{
	counter = 0;
}