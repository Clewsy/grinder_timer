#include <avr/io.h>

#define I2C_SDA_PIN	PC4	// Designated Serial DAta pin.
#define I2C_SCL_PIN	PC5	// Designated Serial CLock pin.
#define I2C_PORT	PORTC	// Port containing SDA and SCL pins.

#define I2C_TWBR	TWBR0	// Bit-Rate Register.
#define I2C_TWCR	TWCR0	// Control Register.
#define I2C_TWDR	TWDR0	// Data Register.
#define I2C_TWSR	TWSR0	// Status Register.
#define I2C_TWEN	TWEN	// Enable Bit.
#define I2C_TWINT	TWINT	// Interrupt Flag Bit.
#define I2C_TWSTA	TWSTA	// Start Condition Bit.
#define I2C_TWSTO	TWSTO	// Stop Condition Bit.

class i2c
{
	public:
		void init(void);		// Initialise the I2C hardware.
		void start(void);		// Set the I2C start condition.
		void stop(void);		// Set the I2C stop condition.
		void write_byte(uint8_t byte);	// Transmit a byte.
		uint8_t read_byte(void);	// Receive a byte.

	private:
		void wait_for_complete(void);	// Loop until the TWI (I2C) Interrupt flag is set indicating an operation has just completed.
};