#include <avr/eeprom.h>

// Timer preset definitions.
#define PRESET_A			0
#define PRESET_B			1
#define PRESET_C			2
#define PRESET_D			3
#define PRESET_MIN_VALUE		16	// Minimum value that can be used for a preset (in sixteenths of a second).
#define PRESET_MAX_VALUE		960	// Maximum value that can be used for a preset (in sixteenths of a second).
#define PRESET_ARRAY_EEPROM_ADDRESS	0x80
#define PRESET_SELECTED_EEPROM_ADDRESS	(PRESET_ARRAY_EEPROM_ADDRESS + 8)

class presets
{
	public:
		uint16_t timer[4] = {0, 0, 0, 0};
		uint8_t selected = PRESET_A;
		void init(void);
		void update_eeprom(void);
};