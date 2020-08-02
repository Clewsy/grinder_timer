#include "pwm.h"

void pwm::init(void)
{
	LED_DDR |= (1 << LED_PIN);		//Set as an output the pin to which the LED is connected.

	LED_TCCR |= (1 << )
}