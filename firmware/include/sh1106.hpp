// Header file for using a 128x64 pixel OLED display with an SH1106 driver.

#include <avr/io.h>
#include <util/delay.h>		// Included as _delay_ms() is used for some animation effects.
#include <avr/pgmspace.h>	// Required for reading from program memory - fonts and bitmaps.
#include "i2c.hpp"		// OLED hardware configured for I2C comms.
#include "sh1106_fonts.h"
#include "sh1106_bitmaps.h"

// SH1106 Address Byte.
#define OLED_ADDR_WRITE		0x78	// 0b01111000 i2c address to write of the OLED SSD1306 driver module with the read/write bit set to write.

// SH1106 Control Bytes.
#define OLED_CTL_LAST_COMMAND	0x00	// 0b00000000 Co=0 D/!C=0 Last control byte, only data bytes to follow.  Following data byte/s are COMMAND operations.
#define OLED_CTL_LAST_RAM	0x40	// 0b01000000 Co=0 D/!C=1 Last control byte, only data bytes to follow.  Following data byte/s are RAM operations.
#define OLED_CTL_CONT_COMMAND	0x80	// 0b10000000 Co=1 D/!C=0 Next two bytes are a data byte, then another control byte.  Data byte is for COMMAND 	operation.
#define OLED_CTL_CONT_RAM	0xC0	// 0b11000000 Co=1 D/!C=1 Next two bytes are a data byte, then another control byte.  Data byte is for RAM operation.

// SH1106 Command Bytes.
#define OLED_SET_CONTRAST				0x81	// Set the contrast.  Following command byte defines contrast value from 0 (low) to 255 (high). (0x7F default).
#define OLED_SET_CONTRAST_DEFAULT			0x7F	// Range from 0x00 (dimest) to 0xFF (brightest).
#define OLED_SET_CONTRAST_BRIGHT			0xFF	// Highest contrast (brightest).
#define OLED_ALL_ON					0xA5	// Turn on all pixels.
#define OLED_ALL_ON_RESUME				0xA4	// Disable "OLED_ALL_ON" (i.e. resume previous graphical output).
#define OLED_INVERSE_DISABLE				0xA6	// Normal mode - setting segment bits turns pixels on.
#define OLED_INVERSE_ENABLE				0xA7	// Inverse mode - setting segment bits turns pixels off.
#define OLED_ON 					0xAF	// Turn on (enable) the oled module.
#define OLED_OFF					0xAE	// Turn off (disable) the oled module.
#define OLED_SET_DISPLAY_START_LINE			0x40	// Set the display start line 0-63.  0x40: line 0 (default).  OR with 6 bits up to 0b111111 for line 63.
#define OLED_SET_SEGMENT_REMAP_COL0			0xA0	// Column address 0 is mapped to SEG 0 (default).
#define OLED_SET_SEGMENT_REMAP_COL127			0xA1	// Column address 127 is mapped to SEG 0. 
#define OLED_SET_MULTIPLEX_RATIO			0xA8	// Set the MUX ratio to N+1 MUX.  Next byte defines MUX from 16-64.  (Default is 0b111111 i.e. 64MUX).
#define OLED_SET_MULTIPLEX_RATIO_DEFAULT		0x3F	// I.e. 64MUX.
#define OLED_SET_COM_OUTPUT_SCAN_DIR_NORMAL		0xC0	// Scan from COM0 to COM[N-1] (N is MUX ratio) (default).
#define OLED_SET_COM_OUTPUT_SCAN_DIR_REVERSE		0xC8	// Scan from COM[N-1] to COM0 (N is MUX ratio).
#define OLED_SET_DISPLAY_OFFSET				0xD3	// Set vertical shift by COM from 0 to 63 defined by next byte.  Default is 0.
#define OLED_SET_DISPLAY_OFFSET_DEFAULT			0x00	// I.e. 0 offset.
#define OLED_SET_COM_PINS_HARDWARE_CONFIG		0xDA	// Set COM pin config with next byte.  Default is 0b00100010 - Alternative COM pin config, disable COM left/right remap.
#define OLED_SET_COM_PINS_HARDWARE_CONFIG_DEFAULT	0x12	// I.e. alternative COM pin config, disable COM left/right remap.
#define OLED_SET_DISPLAY_CLOCK				0xD5	// Set the clock divide ratio/oscillator frequency.  Next byte [7-4] osc freq, [3-0] divide ratio (n +1). 
#define OLED_SET_DISPLAY_CLOCK_DEFAULT			0x00	// I.e. default settings osc freq 0b10000, divide ratio 1.
#define OLED_SET_CHARGE_PUMP				0x8D	// Enable or disable inetrnal charge pump regulator with the next byte.
#define OLED_SET_CHARGE_PUMP_DEFAULT			0x14	// Enable by default - 0x14.  (Use 0x10 to disable).
#define OLED_SET_CHARGE_PERIOD				0xD9	// Set pre-charge and dis-charge periods with next byte.
#define OLED_SET_CHARGE_PERIOD_VALUE			0x33	// Default is 0x22, i.e. Dis-charge period 0x2 cycles, Pre-charge period 0x2 cycles.
//#define OLED_SET_CHARGE_PUMP_VOLTAGE			0x32	// 0x30 OR'd with 0x0 to 0x3 to set 6.4V, 7.4V, 8.0V (default) or 9.0V.
#define OLED_SET_VCOMH_DESELECT_LEVEL			0xDB	// Set Vcomh deselect voltage level with next byte (default 0b00100000 = 0x20).
#define OLED_SET_VCOMH_DESELECT_LEVEL_DEFAULT		0x35	// I.e. ~0.77xVcc.
#define OLED_NO_OP					0xE3	// Command for no operation.
#define OLED_ADDRESS_FIRST_COLUMN_LOWER			0x00	// Command to set lower nibble of column address to first column.  Add lower nibble of column 0 to 127.
#define OLED_ADDRESS_FIRST_COLUMN_HIGHER		0x10	// Command to set higher nibble of column address to first column.  Or with higher nibble of column 0 to 127.
#define OLED_ADDRESS_FIRST_PAGE				0xB0	// Command to set address to first page.  Or with 0 to 7 to set address to pages 0 to 7.

#define SCROLL_UP	1	// Used with the scroll function - will scroll the screen upwards.
#define SCROLL_DN	-1	// Used with the scroll function - will scroll the screen downwards.

// Define class for an oled display with onboard sh1106 driver.
class sh1106
{
	public:
		void init(void);
		void enable_screen(bool enable);
		void clear_screen(void);
		void invert_screen(bool invert);
		void print_char(unsigned char character, const uint8_t *font, uint8_t start_page, uint8_t start_column);
		void print_string(unsigned char *string, const uint8_t *font, uint8_t start_page, uint8_t start_column);
		void draw_box(uint8_t start_row, uint8_t start_column, uint8_t height, uint8_t width);
		void map_bits(const uint8_t *bitmap, const uint16_t bitmap_size);
		void test_pattern(uint8_t pattern);
		void test_pattern(uint8_t pattern, uint8_t delay_ms);
		void scroll(int8_t direction, uint8_t iterations, uint8_t delay);

	private:
		i2c twi;	// i2c class defined in "i2c.h".
		void send_command(uint8_t command);
		void send_command(uint8_t command1, uint8_t command2);
		void send_data(uint8_t data);
		void set_page(uint8_t page);
		void set_column(uint8_t column);
		void set_address(uint8_t page, uint8_t column);
};
