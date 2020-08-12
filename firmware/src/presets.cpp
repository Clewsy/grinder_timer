#include "presets.hpp"

void presets::init(void)
{
	for(uint8_t i = 0; i < 4; i++)	// Cycle through each of the presets A to D.
	{
		timer[i] = eeprom_read_word((uint16_t*)(PRESET_ARRAY_EEPROM_ADDRESS + (2 * i)));	// Set from value saved in eeprom.
		if((timer[i] > PRESET_MAX_VALUE) || (timer[i] < PRESET_MIN_VALUE) || (timer[i] % 4))	// Should be between min and max inclusive and divisible by 4.
		{
			timer[i] = PRESET_MIN_VALUE;									// If invalid, set to min value.
			eeprom_update_word((uint16_t*)(PRESET_ARRAY_EEPROM_ADDRESS + (2 * i)), PRESET_MIN_VALUE);	// And save this valid value.
		}
	}

	selected = eeprom_read_byte((uint8_t*)(PRESET_SELECTED_EEPROM_ADDRESS));		// Set from value saved in eeprom.
	if(selected > PRESET_D)									// Validate the value - should be 0 to 3.
	{
		selected = PRESET_A;								// If invalide, set to PRESET_A.
		eeprom_update_byte((uint8_t*)(PRESET_SELECTED_EEPROM_ADDRESS), PRESET_A);	// And save this valid value.
	}
}

void presets::update_eeprom(void)
{
	for(uint8_t i = 0; i < 4; i++)	eeprom_update_word((uint16_t*)(PRESET_ARRAY_EEPROM_ADDRESS + (2 * i)), timer[i]);	// Save the values of the 4 presets.
	eeprom_update_byte((uint8_t*)(PRESET_SELECTED_EEPROM_ADDRESS), selected);						// Save the currently selected preset.
}
