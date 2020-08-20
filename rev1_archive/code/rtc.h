//Timer/Counter 2 (TC2) will be configured to use an external 32.768kHz crystal and therefore act as a real-time clock (RTC).
//This is desired for accurate timing of the grinder countdown.

#include <avr/io.h>
#include <avr/interrupt.h>
//#include <util/delay.h>	//Only required if the rtc_init includes a delay to wait for oscillation stabilisation.

//Function declarations
void rtc_init(void);
void rtc_enable(void);
void rtc_disable(void);
