#include "pwm.h"

void pwm::init(void)
{
	PWM_DDR |= (1 << PWM_PIN);	//Set as an output the pin to which the LED is connected.

	PWM_TCCRA |= (1 << PWM_COM1);	//Set the Timer/Counter Control Registers (A and B).
	PWM_TCCRA |= (1 << PWM_WGM0);
	PWM_TCCRB |= (1 << PWM_WGM2);
	PWM_TCCRB |= (1 << PWM_CS1);
}

void pwm::enable(void)
{
	PWM_PORT |= (1 << PWM_PIN);	//Set the PWM pin to enable the PWM output.
}

void pwm::disable(void)
{
	PWM_PORT &= ~(1 << PWM_PIN);	//Clear the PWM pin to disable the PWM output.
}

void pwm::set(uint8_t duty)
{
	PWM_SET = duty;		//Set the 8-bit duty cycle value: 0 to 255.
}

uint8_t pwm::get(void)
{
	return PWM_SET;
}