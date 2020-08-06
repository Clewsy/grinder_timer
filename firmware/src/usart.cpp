// Functions used for serial communications via USART

#include <usart.hpp>

//Initialise the hardware to use the USART peripheral.
void usart::init(void)
{
	//Utilising USART0
	UBRR0H = UBRRH_VALUE;	//USART Baud Rate Register 1 High -Value defined in util/setbaud.h
	UBRR0L = UBRRL_VALUE;	//USART Baud Rate Register 1 Low  -Value defined in util/setbaud.h

	#if USE_2X	//Double-Speed detemined in util/setbaud.h.  Needed if defined BAUD not achieavable without U2X1 
		UCSR0A |= (1 << U2X0);		//UCSR0A = USART0 Control and Status Register A
	#else					//U2X0 = Double USART0 Transmission Speed Enable
		UCSR0A &= ~(1 << U2X0);
	#endif
											
	UCSR0B = (1 << RXCIE0) | (1 << TXEN0) | (1 << RXEN0);	//UCSR0B = USART0 Control and Status Register B
								//RXCIE0 = USART0 RX Complete Interrupt Enable
								//TXEN0 = Transmit Enable USART0
								//RXEN0 = Receive Enable USART0

	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);			//UCSR0C = USART 0 Control and Status Register C
								//UCSZ12:0 = USART Character Size, Set to 0b011 for 8-bit.
								//(USBS = Usart Stop Bit Select, Stays at 0b0 for 1 stop bit).
}

//Receive a byte.
uint8_t usart::receive_byte(void)
{
	while (!(UCSR0A & (1 << RXC0))) {}	//Wait until the USART0 receive complete flag is set
	return UDR0;				//Returns received data.  UDR0 = USART0 Data Register
}

//Transmit a byte.
void usart::transmit_byte(uint8_t data)
{
	while (!(UCSR0A & (1 << UDRE0))) {}	//Wait until the USART0 data register is empty (ready to transmit).
       						//Otherwise operates too fast and drops characters.
	UDR0 = data;				//UDR0 = USART0 Data Register
}

//Transmit a series of bytes in the form of ascii characters.
void usart::print_string(char *string)
{			
	uint8_t i = 0;				//Counter to increment for every character in the string.
	while ((string[i]) != '\0')		//Until null character (end of string).
	{
		transmit_byte(string[i]);
		i++;
	}
}

//Convert a byte into the ascii numerical equivalent and print.
void usart::print_byte(uint8_t byte)
{
	while (!(UCSR0A & (1 << UDRE0))) {}		//Wait until the USART 0 data register is empty (ready to transmit).
	transmit_byte('0'+ (byte/100));		//Hundreds
	while (!(UCSR0A & (1 << UDRE0))) {}		//Wait until the USART 0 data register is empty (ready to transmit).
	transmit_byte('0' + ((byte/10) % 10));		//Tens
	while (!(UCSR0A & (1 << UDRE0))) {}		//Wait until the USART 0 data register is empty (ready to transmit).
	transmit_byte('0' + (byte % 10));		//Ones
}

//Convert a byte into the binary and print ascii equivalent.
void usart::print_bin_byte(uint8_t byte)
{
	for (uint8_t bit = 1; bit < 255; bit--)		//For full 8-bits, modify to: "for(bit=7; bit<255; bit--)".
	{
		if (bit_is_set(byte, bit))	transmit_byte('1');
		else				transmit_byte('0');
	}
}
