#include "i2c.h"

//Initialise the I2C hardware.
void i2c_init(void)
{
	TWBR = 2;	//Gives SCL Frequency=400000Hz if F_CPU is 8MHz
			//TWBR: TWI Bit-Rate Register - Used to det the TWI (I2C) Frequency (SCL)
			//SCL frequency = (CPU Freq)/( 16 + 2(TWBR)*(PrescalerValue) )
			//Note, PrescalerValue default = 1, otherwise set in TWSR with bits TWPS[1:0]
			//TWSR: TWI Status Register

	TWCR |= (1 << TWEN);	//TWCR: TWI Control Register
				//TWEN: TWI Enable Bit - turns on TWI (I2C) hardware.
}

//Loop until the TWI (I2C) interrupt flag is set indicating an operation has just completed.
void i2c_wait_for_complete(void)
{
	while(!(TWCR & (1 << TWINT))) {}	//Loop until current TWI (I2C) operation is complete (bit is set).
						//TWINT: TWI Interrupt Flag - Set flag indicates current job has finished.
						//Note, TWINT must be cleared by software - this indicates start of next operation.
						//Extra note, reading TWINT gives status of flag (1=set, 0=clear) but to clear
						//flag, software must write 1 to TWINT.
}

//Set the I2C start condition.
void i2c_start(void)
{
	TWCR = ((1 << TWEN) | (1 << TWSTA) | (1 << TWINT));	//A TWI start is initiated by setting the following bits on TWCR
								//TWEN - TWI enable
								//TWSTA - TWI Start Condition
								//TWINT - Clear TWINT flag (will be set at completion of operation)
	i2c_wait_for_complete();	//Wait until previous operation is complete.
}

//Set the I2C stop condition.
void i2c_stop(void)
{
	TWCR = ((1 << TWEN) | (1 << TWSTO) | (1 << TWINT));	//A TWI start is initiated by setting the following bits on TWCR
								//TWEN - TWI enable
								//TWSTO - TWI Stop Condition
								//TWINT - Clear TWINT flag (will be set at completion of operation)
}

//Transmit a byte.
void i2c_transmit(uint8_t byte)
{
	TWDR = byte;				//TWDR: TWI (I2C) Data Register
	TWCR = ((1 << TWEN) | (1 << TWINT));	//Have to clear the int flag (and keep enabled).
	i2c_wait_for_complete();		//Wait until the int flag is again set.
}
