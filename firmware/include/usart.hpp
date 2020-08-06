// Header file for defining a hardware serial comms receiver/transmitter.

#include <avr/io.h>		//Needed to identify AVR registers and bits.
#include <util/setbaud.h>	//Used to caluculate Usart Baud Rate Register (High and Low) values as a function of F_CPU and BAUD

#ifndef BAUD		//If BAUD isn't already defined,
#define BAUD  9600	//Set BAUD to 9600.
#endif

//Define a USART universal synchronous/asynchronous receiver/transmitter serial peripheral.
class usart
{
	public:
		void init(void);			//Initialise the USART peripheral
		uint8_t receive_byte(void);		//Read in a byte on the USART
		void transmit_byte(uint8_t data);	//Send a byte from the USART
		void print_string(char *string);	//Send a character array as a series of consecutive bytes.
		void print_byte(uint8_t byte);		//Convert a byte value into ascii charactersand print.
		void print_bin_byte(uint8_t byte);	//Convert a byte value into binary equivalent and print the corresponding ascii 1s and 0s.
};