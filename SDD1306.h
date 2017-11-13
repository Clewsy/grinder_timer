#include <avr/io.h>
#include "I2C.h"

#define OLED_WRITE	0b01111000
#define OLED_READ	0b01111001

#define CONTROL_BYTE_COMMAND	0x80
#define CONTROL_BYTE_DATA	0x40

void oled_send_command(uint8_t command);
void oled_send_data(uint8_t data);
