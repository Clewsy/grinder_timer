#include "relay.hpp"

// If not initial state is provided, initialise with relay switched off.
void relay::init(void)
{
	init(false);
}

// Initialise the relay and set the initial state.
void relay::init(bool initial_state)
{
	RELAY_DDR |= (1 << RELAY_PIN); // Configure the relay pin as an output.
	on(initial_state);	       // Set the initial state to off.
}

// Turn on (true) or off (false) the relay.
void relay::on(bool enable)
{
	if(enable)	RELAY_PORT |= (1 << RELAY_PIN);		// Turn the relay on.
	else		RELAY_PORT &= ~(1 << RELAY_PIN);	// Turn the relay off.
}

// Returns true if the relay is on, false if the relay is off.
bool relay::get_status(void)
{
	return((RELAY_PINS & (1 << RELAY_PIN)) >> RELAY_PIN);
}
