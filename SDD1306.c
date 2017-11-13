#include "SDD1306.h"

//SDD1306 OLED controller write mode for I2C: Must follow this packet structure:
//Set I2C start condition
//Send slave address (SLA) 0b011110<SA0> combined with R/W bit set to W (0).  If SA0 pin tied to ground, this byte becomes 0b01111000
//Send the control byte that signals the next byte is either a command (0x80) or data (0x40).
//Send either the actual control byte or the data byte/s.

//#define CONTROL_BYTE_COMMAND	0x80
//#define CONTROL_BYTE_DATA	0x40

void oled_send_command (uint8_t command)
{
	i2c_start();
	i2c_transmit(OLED_WRITE);
	i2c_transmit(CONTROL_BYTE_COMMAND);
	i2c_transmit(command);
	i2c_stop();
}

void oled_send_data (uint8_t data)
{
	i2c_start();
	i2c_transmit(OLED_WRITE);
	i2c_transmit(CONTROL_BYTE_DATA);
	i2c_transmit(data);
	i2c_stop();
}

