#ifndef  F_CPU
#define F_CPU 16000000UL
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>

#include "LCD_driver.h"
#include "I2C_master.h"


uint8_t message1[16] = {' ','8',':','1','4','P','M',' ',' ',' ',' ',' ', '1',':','2','8'};
uint8_t message2[16] = {'M','O','D','E','>',' ',' ','S','E','T','T','I','N','G','S',' '};
uint8_t home_message[16] = {' ',' ',':',' ',' ',' ','M',' ',' ',' ',' ',' ', ' ',':',' ',' '};
/*setupt_backlight_white(Void)
	param - null
	ret - null
	Brief Description:
		This function initializes the RGB light to be white

*/
void set_backlight_white(uint8_t input[1]) {
	uint8_t data[1];
	data[0] = 0x00;
	i2c_writeReg(RGB_ADDR, 0x00, data, 1);
	_delay_ms(1);

	i2c_writeReg(RGB_ADDR, 0x01, data, 1);
	_delay_ms(1);
	
	data[0] = 0xAA;
	i2c_writeReg(RGB_ADDR, 0x08, data, 1);
	_delay_ms(1);
	
	i2c_writeReg(RGB_ADDR, 0x02, input[0], 1);
	_delay_ms(1);

}

/*setupt_LCD(Void)
	param - null
	ret - null
	Brief Description:
		This function initializes the LCD by setting the back-light to be white and configuring
		the desired screen view.
*/
void setup_LCD(void){
	set_backlight_white(0x0F);
	uint8_t data[1];
	data[0] = 0x38;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);
	data[0] = 0x0F;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);
	data[0] = 0x01;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);
	i2c_writeReg(LCD_ADDR, 0x40, home_message, sizeof(message1));
	data[0] = 0xC0;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);//Move to next line 0xC0
	i2c_writeReg(LCD_ADDR, 0x40, message2, sizeof(message2));
	_delay_ms(50);
	//data[0] = 0x01;
	//i2c_writeReg(LCD_ADDR, 0x80, data, 1);//Clear the LCD 0x01
	
}

void update_LCD(void){
	uint8_t data[1];
	data[0] = 0x38;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);
	data[0] = 0x0C;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);
	data[0] = 0x01;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);
	
	i2c_writeReg(LCD_ADDR, 0x40, message1, sizeof(message1));
	data[0] = 0xC0;
	i2c_writeReg(LCD_ADDR, 0x80, data, 1);//Move to next line 0xC0
	i2c_writeReg(LCD_ADDR, 0x40, message2, sizeof(message2));
}

