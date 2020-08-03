#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>	//For using inetrrupt registers and sub-routine vectors.

#define TIMER_TCCRA		TCCR1A	//Timer/Counter Control Register A
#define TIMER_TCCRB		TCCR1B	//Timer/Counter Control Register B
#define TIMER_WGM3		WGM13	//Timer/Counter Waveform Generation Mode Bit 3
#define TIMER_WGM2		WGM12	//Timer/Counter Waveform Generation Mode Bit 2
#define TIMER_WGM1		WGM11	//Timer/Counter Waveform Generation Mode Bit 1
#define TIMER_WGM0		WGM10	//Timer/Counter Waveform Generation Mode Bit 0
#define TIMER_CS2		CS12	//Timer/Counter Clock Select Bit 2
#define TIMER_CS1		CS11	//Timer/Counter Clock Select Bit 1
#define TIMER_CS0		CS10	//Timer/Counter Clock Select Bit 0
#define TIMER_SET_REG		OCR1A	//Timer/Counter Output Compare Register
#define TIMER_SET_VAL		255	//Timer/Counter Output Compare Value
#define TIMER_TIMSK		TIMSK1	//Timer/Counter Timer Interrupt Mask Register
#define TIMER_IE		OCIE1A	//Timer Output Compare Interrupt Enable Bit.
#define TIMER_INT_VECTOR	TIMER1_COMPA_vect

class timer
{
	public:
		void init(void);
		void enable(void);
		void disable(void);
};