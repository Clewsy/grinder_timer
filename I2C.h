#include <avr/io.h>
//These functions are required to utilise the I2C functionality of the AVR (referred to in the AVR datasheet as TWI).
//Note, these functions were written for use with an oled module (SDD1306) that does not have the capability to send data, only receive.
//Therefore I have not written functions for receivind I2C data to the AVR.

//Function declarations
void i2c_init(void);			//Initialise the I2C hardware.
void i2c_wait_for_complete(void);	//Loop until the TWI (I2C) Interrupt flag is set indicating an operation has just completed.
void i2c_start(void);			//Set the I2C start condition.
void i2c_stop(void);			//Set the I2C stop condition.
void i2c_transmit(uint8_t byte);	//Transmit a byte.
