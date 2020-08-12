#include "presets.hpp"

void presets::init(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		timer[i] = eeprom_read_word((uint16_t*)(PRESET_EEPROM_ADDRESS + (2 * i)));
		if(	(timer[i] > PRESET_MAX_VALUE) ||
			(timer[i] < PRESET_MIN_VALUE) ||
			(timer[i] % 4)	)
		{
			timer[i] = PRESET_MIN_VALUE;
			eeprom_update_word((uint16_t*)(PRESET_EEPROM_ADDRESS + (2 * i)), PRESET_MIN_VALUE);
		}
	}
}

void presets::update_eeprom(void)
{
	for(uint8_t i = 0; i < 4; i++)
	{
		eeprom_update_word((uint16_t*)(PRESET_EEPROM_ADDRESS + (2 * i)), timer[i]);
	}
}
