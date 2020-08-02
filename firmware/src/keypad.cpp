#include "keypad.h"

void keypad::init(void)
{
	BUTTON_DDR &= ~BUTTON_MASK;	//((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT));	//Set button pins as inputs.
	BUTTON_PORT |= BUTTON_MASK;	//((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT));	//Enable internal pull-ups.
	BUTTON_PCMSK |= BUTTON_MASK;	//((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT));	//Enable pin-change interrupts for selected pins.
}

void keypad::enable(void)
{
	PCICR |= (1 << BUTTON_PCIE);	//Enable the pin-change interrupt register used by the keypad buttons. PCICR: Pin Change Interrupt Control Register.
}

void keypad::disable(void)
{
	PCICR &= ~(1 << BUTTON_PCIE);	//Enable the pin-change interrupt register used by the keypad buttons. PCICR: Pin Change Interrupt Control Register.
}

//Returns true (i.e. 1) if button is pressed.
bool keypad::state(uint8_t button)
{
	return ((~BUTTON_PINS & (1 << button)) >> button);
}