// Header file to define a generic hardware timer that triggers an interrupt sub-routine.

#include <avr/io.h>
#include <avr/interrupt.h>	// For using interrupt sub-routine vectors.

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

class pulser
{
	public:
		void init(void);
		void enable(bool enable);
		int8_t direction = 1;	// Enables tracking of the LED pulse effect, i.e. 1:getting brighter, -1:getting dimmer.

};