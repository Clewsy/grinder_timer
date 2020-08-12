// Header file to define a generic hardware timer that triggers an interrupt sub-routine.

#include <avr/io.h>
#include <avr/interrupt.h>	// For using interrupt sub-routine vectors.

// Definitions used for initiatilising and controling the pulse timer. 
#define PULSER_TCCRA		TCCR1A			// Timer/Counter Control Register A
#define PULSER_TCCRB		TCCR1B			// Timer/Counter Control Register B
#define PULSER_WGM3		WGM13			// Timer/Counter Waveform Generation Mode Bit 3
#define PULSER_WGM2		WGM12			// Timer/Counter Waveform Generation Mode Bit 2
#define PULSER_WGM1		WGM11			// Timer/Counter Waveform Generation Mode Bit 1
#define PULSER_WGM0		WGM10			// Timer/Counter Waveform Generation Mode Bit 0
#define PULSER_CS2		CS12			// Timer/Counter Clock Select Bit 2
#define PULSER_CS1		CS11			// Timer/Counter Clock Select Bit 1
#define PULSER_CS0		CS10			// Timer/Counter Clock Select Bit 0
#define PULSER_SET_REG		OCR1A			// Timer/Counter Output Compare Register
#define PULSER_TIMSK		TIMSK1			// Timer/Counter Timer Interrupt Mask Register
#define PULSER_IE		OCIE1A			// Timer Output Compare Interrupt Enable Bit.
#define PULSER_INT_VECTOR	TIMER1_COMPA_vect	// Interrupt subroutine name.
#define PULSER_SPEED		8000			// Write to PULSER_SET_REG to set the pulse speed.

// Definitions used for initiatilising and controling the pwm timer. 
#define PWM_PIN			PD2			// In PWM mode, using PD3 as OC3B.
#define PWM_PORT		PORTD			// IO Port that includes the PWM pin.
#define PWM_DDR			DDRD			// Data Direction Register that includes DDR for the PWM pin.
#define PWM_SET			OCR3B			// Output compare register (writing 8-bits to here sets the pulse-width).
#define PWM_TCCRA		TCCR3A			// Timer/Counter Control Register
#define PWM_TCCRB		TCCR3B			// Timer/Counter Control Register
#define PWM_COM0		COM3B0			// Compare Output Mode Bit 0
#define PWM_COM1		COM3B1			// Compare Output Mode Bit 1
#define PWM_WGM0		WGM30			// Waveform Generation Mode Bit 0
#define PWM_WGM1		WGM31			// Waveform Generation Mode Bit 1
#define PWM_WGM2		WGM32			// Waveform Generation Mode Bit 2
#define PWM_WGM3		WGM33			// Waveform Generation Mode Bit 3
#define PWM_CS0			CS30			// Clock Select Bit 0
#define PWM_CS1			CS31			// Clock Select Bit 1
#define PWM_CS2			CS32			// Clock Select Bit 2

// Definitions used by the mode() function.
#define LED_OFF			0b00			// Set LED to off.
#define LED_ON			0b01			// Set LED to on.
#define LED_PULSE		0b11			// Set LED to pulsing.
#define LED_MAX_BRIGHTNESS	150			// Sets the max brightness of the LED when pulsing or on (pwm pulse-width 0 to 255).


class pulser
{
	public:
		void init(uint8_t initial_mode);
		void mode(uint8_t mode);
		void interrupt_handler(void);
	private:
		void enable_pwm(bool enable);
		void enable_pulse(bool enable);
		void set_pwm(uint8_t duty);
		uint8_t get_pwm(void);
		int8_t direction = 1;	// Enables tracking of the LED pulse effect, i.e. 1:getting brighter, -1:getting dimmer.
};