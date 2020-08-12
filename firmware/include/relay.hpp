#include <avr/io.h>

// Relay control definitions.
#define RELAY_PORT		PORTC	// Port containing the pin connected to the relay.
#define RELAY_DDR		DDRC	// Data direction register for the port containing the pin connected to the relay.
#define RELAY_PINS		PINC	// Pins register for reading the state of the relay port.
#define RELAY_PIN		PC0	// Pin connected to the relay coil.
#define RELAY_RESET_DELAY	1000	// Delay in milliseconds after grinding has finished before resetting the timer.


class relay
{
	public:
		void init(void);
		void init(bool initial_state);
		void on(bool enable);
		bool get_status(void);
};