#include "sh1106.hpp"

///////////////////////////////////
///// Begin Private Functions /////
///////////////////////////////////

// Send a single command to oled driver.
void sh1106::send_command(uint8_t command)
{
	twi.start();
	twi.send_byte(OLED_ADDR_WRITE);
	twi.send_byte(OLED_CTL_LAST_COMMAND);
	twi.send_byte(command);
	twi.stop();
}

// Send a double command to oled driver.
void sh1106::send_command(uint8_t command1, uint8_t command2)
{
	twi.start();
	twi.send_byte(OLED_ADDR_WRITE);
	twi.send_byte(OLED_CTL_CONT_COMMAND);
	twi.send_byte(command1);
	twi.send_byte(OLED_CTL_LAST_COMMAND);
	twi.send_byte(command2);
	twi.stop();
}

// Send a single data byte to RAM - i.e. 8-bits to set/clear the 8 pixels at the current address.
void sh1106::send_data(uint8_t data)
{
	twi.start();
	twi.send_byte(OLED_ADDR_WRITE);
	twi.send_byte(OLED_CTL_LAST_RAM);
	twi.send_byte(data);
	twi.stop();
}

// Set page address.  A page is a horizontalk row of 8-bit segments.
// Valid pages are 0 to 7 (top to bottom).
void sh1106::set_page(uint8_t page)
{
	send_command(OLED_ADDRESS_FIRST_PAGE | page);
}

// Set column address.
// Valid columns are 0 to 127 (left to right).
void sh1106::set_column(uint8_t column)
{
	send_command(OLED_ADDRESS_FIRST_COLUMN_LOWER | (column & 0b1111), OLED_ADDRESS_FIRST_COLUMN_HIGHER | ((column >> 4)));
}

// Set the page then column address to be set by the next data write.
// Note, each data write increments the column automatically, but does not change the page.
void sh1106::set_address(uint8_t page, uint8_t column)
{
	set_page(page);
	set_column(column);
}


// Send a segment (8-pixel column) to the ssd1306.
void sh1106::send_segment(uint8_t byte, uint8_t page, uint8_t column)
{
	set_address(page, column);
	send_data(byte);
}


//////////////////////////////////
///// Begin Public Functions /////
//////////////////////////////////

// Initialise the oled display.
void sh1106::init(void)
{
	// Initialise the 12c interface.
	twi.init();

	// Send all the setup commands specific to the hardware configuration.
	send_command(OLED_OFF);
	send_command(OLED_SET_DISPLAY_CLOCK, OLED_SET_DISPLAY_CLOCK_DEFAULT);
	send_command(OLED_SET_MULTIPLEX_RATIO, OLED_SET_MULTIPLEX_RATIO_DEFAULT);
	send_command(OLED_SET_DISPLAY_OFFSET, OLED_SET_DISPLAY_OFFSET_DEFAULT);
	send_command(OLED_SET_DISPLAY_START_LINE | 0x00);
	send_command(OLED_SET_CHARGE_PUMP, OLED_SET_CHARGE_PUMP_DEFAULT);
	send_command(OLED_SET_SEGMENT_REMAP_COL127);
	send_command(OLED_SET_COM_OUTPUT_SCAN_DIR_REVERSE);
	send_command(OLED_SET_COM_PINS_HARDWARE_CONFIG, OLED_SET_COM_PINS_HARDWARE_CONFIG_DEFAULT);
//	send_command(OLED_SET_CONTRAST, OLED_SET_CONTRAST_DEFAULT);
	send_command(OLED_SET_CONTRAST, 0x70);
	send_command(OLED_SET_CHARGE_PERIOD, OLED_SET_CHARGE_PERIOD_VALUE);
	send_command(OLED_SET_VCOMH_DESELECT_LEVEL, OLED_SET_VCOMH_DESELECT_LEVEL_DEFAULT);
	send_command(OLED_ALL_ON_RESUME);
	send_command(OLED_INVERSE_DISABLE);
	send_command(OLED_ON);
}

// Send blanks to every segment of the display.
void sh1106::clear_screen()
{
	send_command(OLED_OFF);		// Turn off the display to hide clearing of segments.
	map_bits(BLANK, sizeof(BLANK));	// Bitmap is a blank.
	send_command(OLED_ON);		// Turn OLED back on again.
}

// Sending true or a one to this function will cause the display to be inverted.
// False or a zero will revert the display to normal/non-inverted.
void sh1106::invert_screen(bool invert)
{
	send_command(OLED_INVERSE_DISABLE + invert);
}

// Take a character, obtain the character map data from the defined font, and print that character to the defined co-ordinates.
// Note, the string must be an array of unsigned characters.  Unsigned allows character integer values up to 255, instead of just 127.
// Values greater than 127 wanted fo this application due to the use of the degrees symbol '°' which has integer value 176.
// This is why passing strings in the format "string" requires typecasting to unsigned char: (unsigned char*)"string"
void sh1106::print_char(unsigned char character, const uint8_t *font, uint8_t start_page, uint8_t start_column)
{
//	uint8_t font_width = pgm_read_byte(&&font[FONT_WIDTH]));		// From the font metadata obtain width of characters in pixels (this is max width for non-fixed-width fonts).
	uint8_t font_height = pgm_read_byte(&font[FONT_HEIGHT]);		// From the font metadata obtain height of characters in pixels.
	uint8_t font_first_char = pgm_read_byte(&font[FONT_FIRST_CHAR]);	// From the font metadata obtain the value of the first included character (often ' ' (i.e. space) or 0x20).
	uint8_t font_num_chars = pgm_read_byte(&font[FONT_NUM_CHARS]);		// From the font metadata obtain the total number of characters included.

	// Determine address of character data for the current character.  This is a 16-bit value split into two 8-bit values stored in PROGMEM.
	// Character address MSB is the byte located at ( FIRST_CHAR_META + ((character - font_first_char) * 4) + CHAR_ADDR_MSB )
	// Note the 16-bit address is indexed from zero where zero is the location of the first character data byte.
	// Therefore actual array address neads to be incremented by the number of bytes used by all metadata.
	//	FIRST_CHAR_META : address of the first character-specific metadata (i.e. skip over the font metadata bytes)
	//	character-font_first_char : convert the char value to the character index number.  E.g. for char '!', 0x21-0x20=0x01, therefore second character in index (0x00 is first).
	//	*4 : Each character has four metadata bytes, so want to skip over 4*character index bytes to get to the metadata for the current character.
	//	+ CHAR_ADDR_MSB : Within the character metadata, the MSB of the address is at index CHAR_ADDR_MSB.
	uint16_t char_data_addr =	( (pgm_read_byte(&font[FIRST_CHAR_META + ((character - font_first_char) * 4) + CHAR_ADDR_MSB]) ) << 8	) +	// Character index MSB shifted left 8 bits +
					( (pgm_read_byte(&font[FIRST_CHAR_META + ((character - font_first_char) * 4) + CHAR_ADDR_LSB]) )	) +	// Character index LSB +
					( FIRST_CHAR_META + (font_num_chars * 4 ) );									// Number of bytes used by font and character metadata.

	// From the char metadata, obtain the number of data bytes used to define the char.
	uint8_t char_data_size = pgm_read_byte(&font[	FIRST_CHAR_META +			// Start at first character metadata byte.
							((character - font_first_char) * 4) +	// Skip to the metadatabytes for the current character.
							CHAR_BYTE_SIZE ]);			// Skip to the metadata byte that represents number of char data bytes.

	// From the char metadata, obtain the char width in pixels. 
	uint8_t char_width = pgm_read_byte(&font[	FIRST_CHAR_META +			// Start at first character metadata byte.
							((character - font_first_char) * 4) +	// Skip to the metadatabytes for the current character.
							CHAR_WIDTH ]);				// Skip to the metadata byte that represents char width.

	// Determine the minimum number of pages needed to display the full height of the character - pages are 8 pixels tall.
	uint8_t char_pages;
	if(font_height % 8)	char_pages = ((font_height + (8 - (font_height % 8))) / 8);	// Height in pixels is not a multiple of 8.
	else			char_pages = (font_height / 8);					// Height in pixels is a multiple of 8.

	// Now have all the information required to print the character.
	for(uint8_t p = 0; p < char_pages; p++)			// Loop trhough each page required for the font height.
	{
		set_address(start_page + p, start_column);	// Move cursor to the start of the current page.
		for(uint8_t c = 0; c < char_width; c++)		// Loop through for each column required for the character width.
		{
			uint16_t seg_address = char_data_addr + (c * char_pages) + p;	// Determine address of the next character byte. 

			if(seg_address < char_data_addr + char_data_size)	send_data(pgm_read_byte(&font[seg_address]));
			else							send_data(0x00);	// Pad remaining font segments with blanks.
		}
	}
}

// Sequentially print each character in a string starting at the defined co-ordinates.
void sh1106::print_string(unsigned char *string, const uint8_t *font, uint8_t start_page, uint8_t start_column)
{
	uint8_t font_first_char = pgm_read_byte(&font[FONT_FIRST_CHAR]);	// From the font metadata obtain the value of the first included character (often ' ' (i.e. space) or 0x20).
	uint8_t column = start_column;						// Initialise the working column - starts at start_column.

	uint8_t character = 0;		// Initialise the character index to be used in the character array.	
	while(string[character])	// Repeat the while loop until "character" is incremented to the end of the string.
	{
		// From the character metadata, determine width in pixels of the current char.
		uint8_t current_char_width = pgm_read_byte(&font[	FIRST_CHAR_META +				// Start at first character metadata byte.
									((string[character] - font_first_char) * 4) +	// Skip to the metadatabytes for the current character.
									CHAR_WIDTH ]);					// Skip to the metadata byte that represents character width.

		print_char(string[character], font, start_page, column);						// Print the current character to the oled.
		column += current_char_width;										// Increment the column index by the width of the character.
		character++;												// Increment to the next character in the string.
	}
}

// Draw a box with top left corner at upper pixel of [start page] and column [start_column],
// with dimensions [width] pixels wide by [height]x8 pixels height (i.e. height is in incrments of page = 8-bits).
void sh1106::draw_box(uint8_t start_page, uint8_t start_column, uint8_t height, uint8_t width)
{
	send_segment(0xFF, start_page, start_column);					// Location of top left box corner.

	for(uint8_t c = start_column; c < (start_column + width - 2); c++)		// Run through top row first page for all columns.
	{
		if(height > 1)	send_data(0b00000001);					// Top row only.
		else		send_data(0b10000001);					// Top and bottom rows (box is a single page in height).
	}

	send_data(0xFF);								// Right-most column, first page.

	if(height > 1)									// Box is more than a single page in height.
	{
		for(uint8_t p = (start_page+ 1 ); p < (start_page + height); p++)	// Run through left- and right-most column, all pages between first and last.
		{
			send_segment(0xFF, p, start_column);				// Left column.
			send_segment(0xFF, p, (start_column + width - 1));		// Right column
		}

		send_segment(0xFF, (start_page + height - 1), start_column);		// Location of bottom left box corner.
		for(uint8_t c = start_column; c < (start_column + width - 2); c++)	// Run through bottom row last page for all columns.
		{
			send_data(0b10000000);						// Bottom row only.
		}
		send_data(0xFF);							// Right-most column, last page. 
	}
}

// Print a full-resolution (128x64) image to screen.
void sh1106::map_bits(const uint8_t *bitmap, const uint16_t bitmap_size)
{
	uint16_t seg = 0;				// seg will increment for every segment sent to the OLED.

	for(uint8_t page = 0; page < 8; page++)		// Loop for each page.
	{
		set_address(page, 0);			// Reset to the first column for each page.

		twi.start();
		twi.send_byte(OLED_ADDR_WRITE);		// Want to write data to the OLED.
		twi.send_byte(OLED_CTL_LAST_RAM);	// All following bytes are data intended for RAM.

		for(uint8_t col = 0; col < 128; col++)	// Loop for every column.
		{
			if(seg < bitmap_size)	twi.send_byte(pgm_read_byte(&bitmap[seg]));
			else			twi.send_byte(0x00);	// Pad remaining gegments with blanks.
			seg++;
		}
		twi.stop();
	}
}

// Print a test pattern to the display.  Will repeat the defined pattern byte for every segment of the display.
void sh1106::test_pattern(uint8_t pattern)
{
	test_pattern(pattern, 0);
}

// Print a test pattern to the display.
// Pause for "delay" milliseconds after every segment for dramatic effect.
void sh1106::test_pattern(uint8_t pattern, uint8_t delay)
{
	for(uint8_t p = 0; p < 8; p++)	// Loop for every page.
	{
		set_address(p, 0);			// Reset to the first column of the current page.

		for(uint8_t c = 0; c < 128; c++)	// Loop for every column.
		{
			send_data(pattern);
			for(uint8_t d = delay; d > 0; d--) _delay_ms(1);
		}
	}
}

// Vertical scrolling effect.
// "direction" is either SCROLL_UP or SCROL_DN.
// "iterations" is how many times to scroll the full screen.
// "delay" is the duration in milliseconds between each offset change.
void sh1106::scroll(int8_t direction, uint8_t iterations, uint8_t delay)
{
	for(uint8_t i = 0; i < iterations; i++)
	{
		// Valid values for "direction" are -1 (=SCROLL_DN) or 1 (SCROLL_UP).
		// Therefore, the following for loop will be equivalent to either of the following:
		// SCROLL_UP:	for(int8_t j = 0; j != 64; j++)		i.e. count from 00 to 64.
		// SCROLL_DN:	for(int8_t j = 64; j != 0; j--)		i.e. count from 64 to 00.
		for(int8_t j = ((direction - 1) * -32); j != ((1 + direction) * 32); j += direction)
		{
			send_command(OLED_SET_DISPLAY_START_LINE | (j + direction));
			for(uint8_t d = delay; d > 0; d--) _delay_ms(1);
		}
	}
}