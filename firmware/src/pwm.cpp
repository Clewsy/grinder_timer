#include "pwm.hpp"

void pwm::init(void)
{
	// Set as an output the pin to which the LED is connected.
	PWM_DDR |= (1 << PWM_PIN);

	// WGM[3:0] set to 0101 : Fast PWM 8-bit.
	// COM[1:0] set to 10 : Clear on compare match, set at bottom (non-inverting mode).
	// CS[2:0] set to 010 : clk/8 (from prescaler).
	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_WGM0));
	PWM_TCCRB |= ((1 << PWM_WGM2) | (1 << PWM_CS1));

	// When initialised, duty cycle will be at a known value.
	PWM_SET = 0x00;
}

void pwm::enable(bool enable)
{
	if(enable)	PWM_PORT |= (1 << PWM_PIN);	// Set the PWM pin to enable the PWM output.
	else		PWM_PORT &= ~(1 << PWM_PIN);	// Clear the PWM pin to disable the PWM output.
}

void pwm::set(uint8_t duty)
{
	// Set the 8-bit duty cycle value: 0 to 255.
	PWM_SET = duty;
}

uint8_t pwm::get(void)
{
	// Return the current duty cycle value: 0 to 255.
	return PWM_SET;
}