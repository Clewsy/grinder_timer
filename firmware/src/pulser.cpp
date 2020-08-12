#include "pulser.hpp"

void pulser::init(uint8_t initial_mode)
{
	////////Initialise the pulse timer.
	// WGM[3:0] set to 0100 : CTC mode, counts from 0 to value of output compare register.
	// COM[1:0] set to 00 : Normal pin modes - not connected to timer.
	// CS[2:0] set to 001 : clk/1 (no prescaling).
	PULSER_TCCRB |= ((1 << PULSER_WGM2) | (1 << PULSER_CS1));
	PULSER_SET_REG = PULSER_SPEED;	// Set the output compare value - the value that triggers the interrupt when reached by the counter.

	////////Initialise the pwm timer.
	// WGM[3:0] set to 0101 : Fast PWM 8-bit.
	// COM[1:0] set to 10 : Clear on compare match, set at bottom (non-inverting mode).
	// CS[2:0] set to 010 : clk/8 (from prescaler).
	PWM_TCCRA |= ((1 << PWM_COM1) | (1 << PWM_WGM0));
	PWM_TCCRB |= ((1 << PWM_WGM2) | (1 << PWM_CS1));
	PWM_DDR |= (1 << PWM_PIN);	// Set as an output the pin to which the LED is connected.
	PWM_SET = 0x00;			// When initialised, duty cycle will be at a known value.

	////////Set the initial led mode.
	mode(initial_mode);
}

// Enable or disable the pwm timer.
void pulser::enable_pwm(bool enable)
{
	if(enable)	PWM_PORT |= (1 << PWM_PIN);	// Set the PWM pin to enable the PWM output.
	else		PWM_PORT &= ~(1 << PWM_PIN);	// Clear the PWM pin to disable the PWM output.
}

// Enable or disable the pulse timer.
void pulser::enable_pulse(bool enable)
{
	if(enable)	PULSER_TIMSK |= (1 << PULSER_IE);	// Enable the output compare interrupt.
	else		PULSER_TIMSK &= ~(1 << PULSER_IE);	// Disable the output compare interrupt.
}

// Set the pwm duty cycle.
void pulser::set_pwm(uint8_t duty)
{
	PWM_SET = duty;		// Set the 8-bit duty cycle value: 0 to 255.
}

// Return the current pwm duty cycle.
uint8_t pulser::get_pwm(void)
{
	return PWM_SET;		// Return the current duty cycle value: 0 to 255.
}

// Set the LED as either on, off or pulsing.
// Valid values for led_mode are:
// LED_OFF	00 (LED disabled, pulse disabled).
// LED_ON	01 (LED enabled, pulse disabled).
// LED_PULSE	11 (LED enabled, pulse enabled).
void pulser::mode(uint8_t mode)
{
	set_pwm(LED_MAX_BRIGHTNESS);
	enable_pwm(mode);
	direction = -1;
	enable_pulse(mode >> 1);
}

// If enable_pulse(), every time the interrupt is triggered, update the brightness (pwm duty cycle) of the LED to create the pulsing effect.
void pulser::interrupt_handler(void)
{
	set_pwm(get_pwm() + direction);							// Update the led brightness.
	if ((get_pwm() == LED_MAX_BRIGHTNESS) || (get_pwm() == 0)) direction *= -1;	// Reverse the pulse direction at either end of the count.
}
