// Header file for defineing a hardware timer/counter to generate a pwm output.

#include <avr/io.h>
#include <avr/interrupt.h>	//For using interrupt sub-routine vectors.

#define PWM_PIN		PD2	//In PWM mode, using PD3 as OC3B.
#define PWM_PORT	PORTD	//IO Port that includes the PWM pin.
#define PWM_DDR		DDRD	//Data Direction Register that includes DDR for the PWM pin.
#define PWM_SET		OCR3B	//Output compare register (writing 8-bits to here sets the pulse-width).
#define PWM_TCCRA	TCCR3A	//Timer/Counter Control Register
#define PWM_TCCRB	TCCR3B	//Timer/Counter Control Register
#define PWM_COM0	COM3B0	//Compare Output Mode Bit 0
#define PWM_COM1	COM3B1	//Compare Output Mode Bit 1
#define PWM_WGM0	WGM30	//Waveform Generation Mode Bit 0
#define PWM_WGM1	WGM31	//Waveform Generation Mode Bit 1
#define PWM_WGM2	WGM32	//Waveform Generation Mode Bit 2
#define PWM_WGM3	WGM33	//Waveform Generation Mode Bit 3
#define PWM_CS0		CS30	//Clock Select Bit 0
#define PWM_CS1		CS31	//Clock Select Bit 1
#define PWM_CS2		CS32	//Clock Select Bit 2

class pwm
{
	public:
		void init(void);
		void enable(void);
		void disable(void);
		void set(uint8_t duty);
		uint8_t get(void);
};

