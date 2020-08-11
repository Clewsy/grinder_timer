#include "pwm.hpp"

void pwm::init(void)
{
	PWM_DDR |= (1 << PWM_PIN);	// Set as an output the pin to which the LED is connected.

	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_WGM0));	// WGM[3:0] set to 0101 : Fast PWM 8-bit.
	PWM_TCCRB |= ((1 << PWM_WGM2) | (1 << PWM_CS1));	// COM[1:0] set to 10 : Clear on compare match, set at bottom (non-inverting mode).
								// CS[2:0] set to 010 : clk/8 (from prescaler).

	PWM_SET = 0x00;			// When initialised, duty cycle will be at a known value.
}

void pwm::enable(bool enable)
{
	if(enable)	PWM_PORT |= (1 << PWM_PIN);	// Set the PWM pin to enable the PWM output.
	else		PWM_PORT &= ~(1 << PWM_PIN);	// Clear the PWM pin to disable the PWM output.
}

void pwm::set(uint8_t duty)
{
	PWM_SET = duty;		// Set the 8-bit duty cycle value: 0 to 255.
}

uint8_t pwm::get(void)
{

	return PWM_SET;		// Return the current duty cycle value: 0 to 255.
}