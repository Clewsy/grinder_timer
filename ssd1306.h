#include <avr/io.h>
#include "i2c.h"
//#include "fonts.h"

//Slave address definitions.  SDD1306 module has slave address 0b011110[SA0][R/W] where:
//-----	SA0 (slave address bit 0) is tied high or low by the hardware.
//-----	R/W (read/write bit) selects whether the next byte will be written by the master to the OLED or read from the OLED by the master.
#define OLED_WRITE	0b01111000	//Slave address of the OLED with W/R bit set to write.
#define OLED_READ	0b01111001	//Slave address of the OLED with W/R bit set to read. Note, read not possible with serial comms.

//Control byte definitions.
#define CONTROL_BYTE_COMMAND	0x80	//Control byte to send that indicates next byte will be a command.
#define CONTROL_BYTE_DATA	0x40	//Control byte to send that indicates next byte will be data.

//Command byte definitions
//Only the commands I have used at some point are listed.  Refer to datasheet for full set.
#define SET_DISPLAY_OFF			0xAE	//Display in "sleep" mode
#define SET_DISPLAY_ON			0xAF	//Display in "normal" mode
#define SET_DISPLAY_CLOCK_DIVIDE	0xD5	//Following byte sets clock divide ratio ([3:0]=0to15) and oscillator frequency ([7:4]=0to15)
#define CLOCK_DIVIDE_DEFAULT		0x80	//Following the SET_DISPLAY_CLOCK_DIVIDE command, this will set the default value (0b10000000);
#define SET_MULTIPLEX_RATIO		0xA8	//Following byte sets the multiplex mode (16to63).  This depends on the display physical size.
#define MULTIPLEX_RATIO_64		0x3F	//Multiplex ratio MUX = this command + 1. For a 128x64 display, this should be d63=0x3F.
#define SET_DISPLAY_OFFSET		0xD3	//Following byte defines the offset for mapping the display start line.  Depends on module config.
#define DISPLAY_OFFSET_0		0x00	//Following the SET_DISPLAY_OFFSET command, this will set the offset to zero which is the default. 
#define SET_DISPLAY_START_LINE_0	0x40	//0x40 to 0x7F defines the start line from 0 to 63.  I.e. bits [5:0] in 0b01XXXXXX give the start line 0 to 63.
#define CHARGE_PUMP_CONTROL		0x8D	//Configure charge pump settings.  Use of SSD1306 charge pump circuit depends on module config.
#define ENABLE_CHARGE_PUMP		0x14	//Following CHARGE_PUMP_CONTROL, this byte will enable the charge pump.
#define SET_MEMORY_ADDRESSING_MODE	0x20	//Following byte will define the memory addressing mode to be used (page, horizontal or vertical).
#define PAGE_ADDRESS_MODE		0x02	//Following the SET_MEMORY_ADDRESSING_MODE, this byte will put the module into page addressing mode (this is the default).
#define SET_SEGMENT_REMAP_0		0xA0	//Segment re-mamp defines which column address is mapped to SEG0 (first data byte).  This is the default on RESET.
#define SET_SEGMENT_REMAP_127		0xA1	//This segment re-map defines column address 127 as SEG0, effectively a horizontal flip.
#define SET_COM_OUTPUT_SCAN_DIR_NORMAL	0xC0	//This sets the scan direction from COM0 to COM[N-1] (default on RESET).
#define SET_COM_OUTPUT_SCAN_DIR_REVERSE	0xC8	//This reverses the scan direction from COM[N-1] to COM0 (effectively a vertical flip).
#define SET_COM_PINS_HARDWARE_CONFIG	0xDA	//Next byte will set the pin configuration to match the module hardware layout. 
#define COM_PINS_HARDWARE_DEFAULT	0x12	//If following the SET_COM_PINS_HARDWARE_CONFIG byte, this will apply the default hardware settings.
#define SET_DISPLAY_CONTRAST		0x81	//Following byte will define the contrast level (0x00 to 0xFF)
#define CONTRAST_MAX			0xFF	//Value corresponds to maximum contrast level.
#define CONTRAST_HIGH			0xC0	//Value corresponds to high contrast level.
#define CONTRAST_MED			0x80	//Value corresponds to medium contrast level.
#define CONTRAST_LOW			0x40	//Value corresponds to low contrast level.
#define CONTRAST_MIN			0x00	//Value corresponds to minimum contrast level.
#define ENTIRE_DISPLAY_ON		0xA5	//Turns on every pixel regardless of what's in RAM (though the RAM data will remain unaltered).	
#define RESUME_FROM_ENTIRE_DISPLAY_ON	0xA4	//Resumes displaying from RAM data.
#define SET_DISPLAY_NOT_INVERSE		0xA6	//Default, display is not inverted.
#define SET_DISPLAY_INVERSE		0xA7	//Inverts every pixel.

#define SET_LOW_COLUMN_START_ADDRESS	0x00	//Combine with the next command to give the full byte indicating the column start address.  "OR" 0x00 with 4 bits to give the lower nibble  
#define SET_HIGH_COLUMN_START_ADDRESS	0x10	//and "OR" 0x10 with 4 bits to give the upper nibble.  I.e. (0x00 | 0b0000EFGH),  (0x01 | 0b0000ABCD) gives column start address 0bABCDEFGH.
#define SET_PAGE_START_ADDRESS		0xB0	//This command by itself sets the page start address to page 0. "OR" with 0x0 to 0x7 to set pages 0 to 7 (I.e. page range is 0xB0 to 0xB7)

//Notes on page addressing mode (for 64x128 display):
//Address Pointer Movement
//		Col0	Col1	...	Col126	Col127
//	Page0	1stB	->	->	->	to Col0
//	...	->	->	->	->	...
//	Page6	->	->	->	->	to Col0
//	Page7	->	->	->	->	to Col0
//
//Each page consists of 128 vertical bytes with LSB at the top.
//Each byte is effectively a vertical line of 8 pixel:
//e.g. 	Byte/		b0	<-LSb
//	Segment:	b1
//			b2
//			b3
//			b4
//			b5
//			b6
//			b7	<-MSb
//Every time a byte is written, column address increments but page address must me set manually.

//function declarations
void oled_send_command(uint8_t command);
void oled_send_data(uint8_t data);
void oled_init(void);
void oled_set_address(uint8_t column, uint8_t page);
void oled_clear_screen(void);
void oled_test_pattern(void);
void oled_draw_box(uint8_t column, uint8_t page, uint8_t width, uint8_t height);
void oled_type_char(char character);
void oled_type_string(char string[]);
void oled_type_byte(uint8_t byte);
