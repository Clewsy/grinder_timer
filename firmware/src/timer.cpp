#include "timer.h"

void timer::init(void)
{
	TIMER_TCCRB |= ((1 << TIMER_WGM2) | (1 << TIMER_CS2) | (1 << TIMER_CS0));
	TIMER_SET_REG = TIMER_SET_VAL;
}

void timer::enable(void)
{
	TIMER_TIMSK |= (1 << OCIE1A);
}

void timer::disable(void)
{
	TIMER_TIMSK &= ~(1 << OCIE1A);
}
