#include "clock.hpp"
#include "keypad.hpp"
#include "presets.hpp"
#include "pulser.hpp"
#include "relay.hpp"
#include "sh1106.hpp"
#include "sleeper.hpp"
//#include "usart.hpp"

////////////////////////////////////////
// Initialise the various classes used.
//usart serial;		// Serial interface - really only used for debugging.
keypad buttons;		// 5-button keypad.
sleeper sleep_timer;	// (Uses Timer/Counter 0) Trigger an interrupt after a set duration to enter a "sleep" mode.
pulser led;		// (Uses Timer/Counter 1 for pulsing, Timer/Counter 3 for pwm) LED connected to a pwm output can be on, off or pulsing.
clock rtc;		// (Uses Timer/Counter 2) A real-time clock (using an external 32.768kHz crystal) is used for acurate timing.
sh1106 oled;		// A 128x64 pixel oled with a sh1106 driver.
presets preset;		// Timer values for four presets, plust the currently selected preset.  Retained through a power-cycle by being saved in eeprom.
relay grinder;		// An output pin connected to a relay.  In this case, the relay will control the grinder motor.

////////////////////////////////////////
// Function declarations.
ISR(SLEEPER_INT_VECTOR);			// Interrupt subroutine triggered by the sleep mode timer.
ISR(BUTTON_PCI_VECTOR);				// Interrupt subroutine triggered by a button press.
ISR(TIMER_INT_VECTOR);				// Interrupt subroutine triggered by the LED pulse effect timer.
ISR(CLOCK_INT_VECTOR);				// Interrupt subroutine triggered by the real-time clock tiomer.
void led_control(uint8_t led_mode);		// Set the led to either off, on or pulsing.
void sleep_mode(bool go_to_sleep);		// Enable or cancel "sleep mode".
void change_preset_value(int8_t up_or_down);	// Increase or decrease the value of the current selected preset.
void handle_up_down(int8_t up_or_down);		// Handle pressing or holding the up or down buttons.
void handle_left_right(int8_t left_or_right);	// Scroll left or right through the different presets.
void grind(bool grind);				// Start or stop the grinder.
void refresh_menu(void);			// Update the preset selection icons to show which preset is currently selected.
void refresh_timer(void);			// Update the countdown timer section of the oled.
void refresh_display(void);			// First run refresh_menu() then refresh_timer().
void splash(void);				// Silly animation that runs at poer on.
void hardware_init(void);			// Initialise peripherals.
int main(void);					// contains the infinite loop.
