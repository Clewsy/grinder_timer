//Definitions and declarations used for serial communications via USART

#ifndef BAUD		//If BAUD isn't already defined,
#define BAUD  9600	//Set BAUD to 9600
#endif

#include <avr/io.h>		//Needed to identify AVR registers and bits.
#include <util/setbaud.h>	//Used to caluculate Usart Baud Rate Register (High and Low) values as a function of F_CPU and BAUD

//Function declarations
void initUSART(void);			//Initialise the USART peripheral
uint8_t receiveByte(void);		//Sets a variable to whatever was received by the USART
void transmitByte(uint8_t data);	//Transmits a byte from the USART
void printString(const char string[]);	//Transmits a string of characters.
void printByte(uint8_t byte);		//Takes an integer and transmits the characters.
void printBinaryByte(uint8_t byte);	//Takes an integer and prints the binary equivalent.
