// Header file for configuring an asynchronous hardware timer to operate as a real-time clock.
// Uses an externally connected 32.768kHz crystal oscillator.

#include <avr/io.h>
#include <avr/interrupt.h>	//For using interrupt sub-routine vectors.

#define CLOCK_TIMSK	TIMSK2	// Timer/Counter 2 Interrupt Mask Register.
#define CLOCK_ASSR	ASSR	// Asynchronous Status Register.
#define CLOCK_TCNT	TCNT2	// Timer/Counter 2 Value Register.
//#define CLOCK_TCCRA	TCCR2A	// Timer/Counter 2 Control Register A.
#define CLOCK_TCCRB	TCCR2B	// Timer/Counter 2 Control Register B.

#define CLOCK_OCIEA	OCIE2A	// TIMSK2 Output Compare Interrupt Enable Channel A Bit.
#define CLOCK_OCIEB	OCIE2B	// TIMSK2 Output Compare Interrupt Enable Channel B Bit.
#define CLOCK_TOIE	TOIE2	// TIMSK2 Timer Overflow Interrupt Enable Bit.  Enables ISR(TIMER2_OVF_vect).
#define CLOCK_AS	AS2	// ASSR Asynchronous Timer/Counter 2 Clock Source Bit. 
#define CLOCK_TCUB	TCN2UB	// ASSR Timer/Counter 2 Value Register Update Busy Bit.
#define CLOCK_OCRAUB	OCR2AUB	// ASSR Timer/Counter 2 Output Compare Register Update Busy Bit.
#define CLOCK_OCRBUB	OCR2BUB	// ASSR Timer/Counter 2 Output Compare Register Update Busy Bit.
#define CLOCK_TCRAUB	TCR2AUB	// ASSR Timer/Counter 2 Control Register Update Busy Bit.
#define CLOCK_TCRBUB	TCR2BUB	// ASSR Timer/Counter 2 Control Register Update Busy Bit.
//#define CLOCK_COMA1	COM2A1	// TCCR2A Compare Output Mode Channel A Bit 1.
//#define CLOCK_COMA0	COM2A0	// TCCR2A Compare Output Mode Channel A Bit 0.
//#define CLOCK_COMB1	COM2A1	// TCCR2A Compare Output Mode Channel B Bit 1.
//#define CLOCK_COMB0	COM2A0	// TCCR2A Compare Output Mode Channel B Bit 0.
#define CLOCK_CS2	CS22	// TCCR2B Clock Select Bit 2.
#define CLOCK_CS1	CS21	// TCCR2B Clock Select Bit 1.
#define CLOCK_CS0	CS20	// TCCR2B Clock Select Bit 0.

#define CLOCK_INT_VECTOR	TIMER2_OVF_vect	// Interrupt Sub-Routine ISR vector.

class clock
{
	public:
		void init(void);
		void enable(bool enable);
		uint16_t counter = 0;
};