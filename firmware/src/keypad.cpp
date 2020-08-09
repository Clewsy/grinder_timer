#include "keypad.hpp"

// Initialise the keypad hardware.
void keypad::init(void)
{
	BUTTON_DDR &= ~BUTTON_MASK;	// Set button pins as inputs.
	BUTTON_PORT |= BUTTON_MASK;	// Enable internal pull-ups.
	BUTTON_PCMSK |= BUTTON_MASK;	// Enable pin-change interrupts for selected pins.
}

// Effectively enables the keypad by enabling the pin-change interrupt. 
void keypad::enable(void)
{
	PCICR |= (1 << BUTTON_PCIE);	// PCICR: Pin Change Interrupt Control Register.
}

// Effectively disables the keypad by disabling the pin-change interrupt. 
void keypad::disable(void)
{
	PCICR &= ~(1 << BUTTON_PCIE);	// PCICR: Pin Change Interrupt Control Register.
}

// Returns true (i.e. 1) if "button" is pressed.
bool keypad::check(uint8_t button)
{
	return ((~BUTTON_PINS & (1 << button)) >> button);
}

// Returns true (i.e. 1) if any button is pressed.
bool keypad::any(void)
{
	if(~BUTTON_PINS & BUTTON_MASK)	return(true);
	else				return(false);
}
