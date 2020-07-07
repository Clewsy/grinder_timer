
#include "rtc.h"

//Initialise the hardware as required.
void rtc_init(void)
{
	//_delay_ms(1000);	//A 32.768kHz crystal requires up to one second to stabilize after power-up.
				//I have disabled this delay as there is a delay used at power-up to display a splash-screen.

	TIMSK2 &= ~((1 << OCIE2B) | (1 << OCIE2A) | (1 << TOIE2));	//Ensure all TC2 interrupts are disabled during configuration.
									//TIMSK2: Timer/Counter 2 Interrupt Mask Register.
									//OCIE2B: TC2 Output Compare B Match Interrupt Enable.
									//OCIE2A: TC2 Output Compare A Match Interrupt Enable.
									//TOIE2: TC2 Overflow Interrupt Enable.

	ASSR |= (1 << AS2);	//Set TC2 to use the external clock source on pins TOSC1 & TOSC2.
				//ASSR: Asynchronous Status Register.
				//AS2: Asynchronous Timer/Counter 2.

	TCNT2 = 0;		//Reset the counter.
				//TCNT2: TC2 Counter Value Register.

	TCCR2B |= (1 << CS21);	//Set the frequency prescaler to control how often the counter "ticks".
				//TCCR2B: TC2 Control Register.
				//CS2[2:0]: TC2 Clock Select.
				//For CS2[2:0] = 0b010, TCNT2 overflows every 1/16th (0.0625) seconds.
				//I.e. with prescaler set to 8, counter ticks at a frequency of clk/8 = 32.768kHz/8 = 4096Hz.
				//Therefore the counter overflows every 256/4096 seconds = 1/16 seconds.

	//Wait until writing to all TC2 registers is complete.
	while(ASSR & ((1 << TCN2UB) | (1 << OCR2AUB) | (1 << OCR2BUB) | (1 << TCR2AUB) | (1 << TCR2BUB))){}
				//ASSR: Asynchronous Status Register.
				//TCN2UB: TC2 Update Busy - Set until write to TCNT2 is complete.
				//TCR2AUB: TC2 Control Register A Update Busy - Set until write to TCCR2A is complete.
				//TCR2BUB: TC2 Control Register B Update Busy - Set until write to TCCR2B is complete.
				//OCR2AUB: TC2 Output Compare Register A Update Busy - Set until write to OCR2A is complete.
				//OCR2BUB: TC2 Output Compare Register B Update Busy - Set until write to OCR2B is complete.

	//Following could be used in a general-purpose RTC initialisation but, for this application it serves as the separate rtc_enable/disable functions.
	//TIMSK2 |= (1 << TOIE2);	//Enable required interrupts:
					//TOIE2: TC2 Overflow Interrupt.  This will trigger "ISR(TIMER2_OVF_vect)" when TCNT2 overflows.

	//Global enable of interrupts.  Can be included here if not enabled elsewhere in the code.
	//sei();
}

//Function to effectively enable the timer by enabling the overflow interrupt (assumes rtc has otherwise been initialise).
void rtc_enable(void)
{
	TIMSK2 |= (1 << TOIE2);	//Enable overflow interrupt.
				//TOIE2: TC2 Overflow Interrupt.  This will trigger "ISR(TIMER2_OVF_vect)" when TCNT2 overflows.
}

//Function to effectively disable the timer by disabling the overflow interrupt.
void rtc_disable(void)
{
	TIMSK2 &= ~(1 << TOIE2);	//Disable overflow interrupt TOIE2.
}
