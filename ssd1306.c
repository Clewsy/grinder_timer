#include "ssd1306.h"

//SDD1306 OLED controller write mode for I2C: Must follow this packet structure:
//Set I2C start condition
//Send slave address (SLA) 0b011110<SA0> combined with R/W bit set to W (0).  If SA0 pin tied to ground, this byte becomes 0b01111000
//Send the control byte that signals the next byte is either a command (0x80) or data (0x40).
//Send either the actual control byte or the data byte/s.

//Using the I2C hardware, send a command packet.  This must include the control byte that indicates a command.
void oled_send_command(uint8_t command)
{
	i2c_start();				//Set I2C start condition
	i2c_transmit(OLED_WRITE);		//Transmit slave address & write bit.
	i2c_transmit(CONTROL_BYTE_COMMAND);	//Transmit control byte to indicate a command.
	i2c_transmit(command);			//Transmit the command byte.
	i2c_stop();				//Set the I2C stop condition.
}

//Using the I2C hardware, send a data packet.  This must include the control byte that indicates data.
void oled_send_data(uint8_t data)
{
	i2c_start();				//Set I2C start condition.
	i2c_transmit(OLED_WRITE);		//Transmit slave address & write bit.
	i2c_transmit(CONTROL_BYTE_DATA);	//Transmit control byte to indicate data.
	i2c_transmit(data);			//Transmit the data.
	i2c_stop();				//Set the I2C stop condition.
}

//Initialise the SDD1306 OLED driver.  This listing follows the suggested initialisation flow from the datasheet.
//A lot of these commands are the defaults anyway, but it doesn't hurt to include them.
//Refer to the .h file for descriptions or the datasheet for detail.
void oled_init(void)
{
	oled_send_command(SET_DISPLAY_OFF);
	oled_send_command(SET_MULTIPLEX_RATIO);
	oled_send_command(MULTIPLEX_RATIO_64);
	oled_send_command(SET_DISPLAY_OFFSET);
	oled_send_command(DISPLAY_OFFSET_0);
	oled_send_command(SET_DISPLAY_START_LINE_0);	
	oled_send_command(SET_SEGMENT_REMAP_0);
	oled_send_command(SET_COM_OUTPUT_SCAN_DIR_NORMAL);
	oled_send_command(SET_COM_PINS_HARDWARE_CONFIG);
	oled_send_command(COM_PINS_HARDWARE_DEFAULT);
	oled_send_command(SET_DISPLAY_CONTRAST);
	oled_send_command(CONTRAST_HIGH);
	oled_send_command(RESUME_FROM_ENTIRE_DISPLAY_ON);
	oled_send_command(SET_DISPLAY_NOT_INVERSE);
	oled_send_command(SET_DISPLAY_CLOCK_DIVIDE);
	oled_send_command(CLOCK_DIVIDE_DEFAULT);
	oled_send_command(CHARGE_PUMP_CONTROL);
	oled_send_command(ENABLE_CHARGE_PUMP);
	oled_send_command(SET_DISPLAY_ON);
}

//This function sets the address to  which oled image data will be written.
//Note, in page address mode, the column is automatically incremented with every data byte writte, but the page must be set manually.
//Consider the column to be the x co-ords (0to127) and the page to be the y co-ords (0 to 7).
//Each data byte is referred to as a segment and effectively represents a vertical line of 8 pixels. 
void oled_set_address(uint8_t column, uint8_t page)
{
	oled_send_command(SET_LOW_COLUMN_START_ADDRESS | (column & 0x0F));	//Send the byte that sets the column address lower nibble.
	oled_send_command(SET_HIGH_COLUMN_START_ADDRESS | (column >> 4));	//Send the byte that sets the column address higher nibble.
	oled_send_command(SET_PAGE_START_ADDRESS | page);			//Send the byte that sets the page address.
}

//Set every data bit to zero in order to clear the screen.
void oled_clear_screen(void)
{
	uint8_t i,j;
	for(i=0;i<8;i++)			//i cycles from 0 to 7 to write to each page.
	{
		oled_set_address(0,i);		//Only the page needs to be addressed manually as the column increments with every data byte sent.
		for(j=0;j<128;j++)		//j cycles from 0 to 127 for each column.
		{
			oled_send_data(0x00);	//Send all 0s to clear the pixels.
		}
	}
}

//Draw a test pattern - a series of diagonal lines across the screen.
void oled_test_pattern(void)
{
	unsigned char i,j,k;
	for (k=0;k<8;k++)
	{
		oled_set_address(0,k);
		j=0;
		for(i=0;i<128;i++)
		{
			if(j==8){j=0;}
			oled_send_data(1 << j);
			j++;
		}
	}
}

//Draws a box at outside edges of segments starting at <column>, <page> and of size <width> columns and <height pages>.
void oled_draw_box(uint8_t column, uint8_t page, uint8_t width, uint8_t height)
{
	oled_set_address(column,page);
	oled_send_data(0xFF);					//Forms first vertical line for first segment in first page.
	uint8_t i;
	for(i=column;i<(column+width-2);i++)
	{	
		if(height != 1)
		{
			oled_send_data(0x01);			//Forms top horizontal line if height>1 page.
		}
		else
		{
			oled_send_data(0xC1);			//Forms the top and bottom horizontal lines for a box height of 1 page.
		}
	}
	oled_send_data(0xFF);					//Forms right vertical line for last segment in first page.

	if(height != 1)						//If box height is only one page then the box is done.  Else the following is required for side and bottom lines.
	{
		for(i=page+1;i<(page+height);i++)
		{
			oled_set_address(column,i);
			oled_send_data(0xFF);			//Forms left vertical line.
			oled_set_address(column+width-1,i);
			oled_send_data(0xFF);			//Forms right vertical line.
		}

		oled_set_address(column,page+height-1);
		oled_send_data(0xFF);				//Forms left vertical line for first segment in last page.
		for(i=column;i<(column+width-2);i++)
		{
			oled_send_data(0xC0);			//Forms bottom horizontal line.
		}
		oled_send_data(0xFF);				//Forms right vertical line for last segment in last page.
	}
}

