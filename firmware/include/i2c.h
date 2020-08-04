#include <avr/io.h>

#define I2C_TWBR	TWBR0	// Two-Wire Interface Bit-Rate Register.
#define I2C_TWCR	TWCR0	// Two-Wire Interface Control Register.
#define I2C_TWDR	TWDR0	// Two-Wire Interface Data Register.
#define I2C_TWEN	TWEN	// Two-Wire Interface Enable Bit.
#define I2C_TWINT	TWINT	// Two-Wire Interface Interrupt Flag Bit.
#define I2C_TWSTA	TWSTA	// Two-Wire Interface Start Condition Bit.
#define I2C_TWSTO	TWSTO	// Two-Wire Interface Stop Condition Bit.

class i2c
{
	public:
		void init(void);		//Initialise the I2C hardware.
		void start(void);		//Set the I2C start condition.
		void stop(void);		//Set the I2C stop condition.
		void transmit(uint8_t byte);	//Transmit a byte.

	private:
		void wait_for_complete(void);	//Loop until the TWI (I2C) Interrupt flag is set indicating an operation has just completed.
};