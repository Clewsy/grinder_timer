#include <avr/io.h>

void i2c_init(void);
void i2c_wait_for_complete(void);
void i2c_start(void);
void i2c_stop(void);
void i2c_transmit(uint8_t byte);
