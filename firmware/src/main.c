#include "usart.h"

#define BUTTON_PORT	PORTD
#define BUTTON_PINS	PIND
#define BUTTON_DDR	DDRD
#define BUTTON_LED	PD2
#define BUTTON_UP	PD3
#define BUTTON_DOWN	PD4
#define BUTTON_LEFT	PD5
#define BUTTON_RIGHT	PD6
#define BUTTON_GRIND	PD7

#define RELAY_PORT	PORTC
#define RELAY_DDR	DDRC
#define RELAY_PIN	PC0


int main(void)
{

usart_init();

BUTTON_PORT |= ((1 << BUTTON_GRIND) | (1 << BUTTON_UP) | (1 << BUTTON_DOWN) | (1 << BUTTON_LEFT) | (1 << BUTTON_RIGHT));	//Enable internal pull-ups.
BUTTON_DDR |= (1 << BUTTON_LED);		//Set as an output the pin to which the LED is connected.

RELAY_DDR |= (1 << RELAY_PIN);

uint8_t i=0;

	while (1)
	{
//		usart_print_string("testing 123\r\n");
//		usart_print_byte(i);
//		i++;
		
//		if(i>127)	PORTD |= (1 << PD2);
//		else		PORTD &= ~(1 << PD2);
		
		if(~BUTTON_PINS & (1 << BUTTON_UP))
		{
			usart_print_string("UP\r\n");
			BUTTON_PORT &= ~(1 << BUTTON_LED);
//			RELAY_PORT &= ~(1 << RELAY_PIN);
		}
		if(~BUTTON_PINS & (1 << BUTTON_DOWN))	usart_print_string("DOWN\r\n");
		if(~BUTTON_PINS & (1 << BUTTON_LEFT))	usart_print_string("LEFT\r\n");
		if(~BUTTON_PINS & (1 << BUTTON_RIGHT))	usart_print_string("RIGHT\r\n");
		if(~BUTTON_PINS & (1 << BUTTON_GRIND))
		{
			usart_print_string("GRIND\r\n");
			BUTTON_PORT |= (1 << BUTTON_LED);
//			RELAY_PORT |= (1 << RELAY_PIN);
		}	
			
		
		
	}

	return 0;	//Never reached.
}
