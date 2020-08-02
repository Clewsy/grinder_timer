//Header file for keypad - pin-change interrupt triggering buttons.

#include <stdint.h>
#include <avr/io.h>
#include <avr/interrupt.h>	//For using inetrrupt registers and sub-routine vectors.
#include <util/delay.h>		//Needed to use _delau_ms() for de-bouncing buttons.


#define BUTTON_PORT		PORTD		//Register for setting button pull-ups.
#define BUTTON_PINS		PIND		//Register for reading button states.
#define BUTTON_DDR		DDRD		//Register for setting data direction for buttos (inputs not outputs).
#define BUTTON_PCIE		PCIE2		//Register for enabling pin-change interrupt for buttons.
#define BUTTON_PCMSK		PCMSK2		//Register for masking which pins shound trigger the pin-change interrupt.
#define BUTTON_PCI_VECTOR	PCINT2_vect	//Sub-routine vector - will run when triggered by button pin-change interrupt. 

#define BUTTON_DEBOUNCE_MS	10	//Button de-bounce duration in milliseconds.

#define BUTTON_UP	PD3	//Pin connected to the up button
#define BUTTON_DOWN	PD4	//Pin connected to the down button
#define BUTTON_LEFT	PD5	//Pin connected to the left button
#define BUTTON_RIGHT	PD6	//Pin connected to the right button
#define BUTTON_GRIND	PD7	//Pin connected to the grind button
#define BUTTON_MASK	((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT))


class keypad
{
	public:
		void init(void);
		void enable(void);
		void disable(void);
		bool state(uint8_t button);
};

