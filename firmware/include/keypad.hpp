// Header file for keypad - pin-change interrupt triggering buttons.
#include <avr/io.h>

#define BUTTON_PORT		PORTD		// Register for setting button pull-ups.
#define BUTTON_PINS		PIND		// Register for reading button states.
#define BUTTON_DDR		DDRD		// Register for setting data direction for buttos (inputs not outputs).
#define BUTTON_PCIE		PCIE2		// Register for enabling pin-change interrupt for buttons.
#define BUTTON_PCMSK		PCMSK2		// Register for masking which pins shound trigger the pin-change interrupt.
#define BUTTON_PCI_VECTOR	PCINT2_vect	// Sub-routine vector - will run when triggered by button pin-change interrupt. 
#define BUTTON_UP		PD3		// Pin connected to the up button
#define BUTTON_DOWN		PD4		// Pin connected to the down button
#define BUTTON_LEFT		PD5		// Pin connected to the left button
#define BUTTON_RIGHT		PD6		// Pin connected to the right button
#define BUTTON_GRIND		PD7		// Pin connected to the grind button
#define BUTTON_MASK		((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT))

#define BUTTON_DEBOUNCE_MS	10		// Button de-bounce duration in milliseconds.
#define BUTTON_LONG_PRESS	500		// Duration (in ms approximately) to wait before registering a long-press.
#define BUTTON_FAST_CHANGE	50		// Duration (in ms approximately) between actions when the button is held in. (long-press)
#define UP			1		// Value indicates up button - used in handle_up_or_down() and change_preset_value() functions.
#define DOWN			-1		// Value indicates down button - used in handle_up_or_down() and change_preset_value() functions.
#define LEFT			-1		// Value indicates left button - used in handle_left_or_right() function.
#define RIGHT			1		// Value indicates right button - used in handle_left_or_right() function.

class keypad
{
	public:
		void init(void);
		void enable(void);
		void disable(void);
		bool check(uint8_t button);
		bool any(void);
};

