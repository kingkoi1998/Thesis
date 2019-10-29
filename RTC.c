

#include <stdio.h>
#include <avr/io.h>
#include <stdint.h>

#include "i2c_master.h"
#include "RTC.h"
#include "LCD_driver.h"

#define DISABLE_BIT(ADDR, BIT) reg_write((ADDR), ~((~(reg_read(ADDR)))|(1<<(BIT))))

#define ENABLE_BIT(ADDR, BIT) reg_write((ADDR), (reg_read(ADDR)|(1<<(BIT))))

uint8_t reg_read(uint8_t address);
void reg_write(uint8_t address, uint8_t data);

uint8_t rtc_get_second();
uint8_t rtc_get_minute();
uint8_t rtc_get_hour();

uint8_t rtc_binary_to_decimal(uint8_t binary);
uint8_t rtc_decimal_to_binary(uint8_t decimal);


/**
 * Initialize the RTC oscillator and select 24 hour representation.
 */
void init_rtc()
{
	//reg_write(0x07, 0x00);
	//ENABLE_BIT(0x03, 3);
	////if(reg_read(0x03) & (1<<4)) {
		////DISABLE_BIT(0x03, 4);
	////}
	//ENABLE_BIT(0x00, 7);
	i2c_start(MCP_write);
	i2c_write(0x03);
	i2c_write(0x08);
	i2c_stop();
}


/**
 * Read data in a second register in decimal format.
 */
uint8_t rtc_get_second()
{
	//return data in a second register.
	return rtc_binary_to_decimal(reg_read(0x00) & 0x7F);
}
/**
 * Set a second register value with given decimal value.
 *
 * @param s is a decimal value for second.
 */
void rtc_set_second(uint8_t s)
{
	//convert a given decimal second value into decimal for register.
	s = rtc_decimal_to_binary(s);
	s = s|0x80;
	//start writing communication
	i2c_start(MCP_write);
	//points a second register in RTC
	i2c_write(0x00);
	//write a given value.
	i2c_write(s);
	//stop writing communication
	i2c_stop();
}

/**
 * Read data in a minute register in decimal format.
 */
uint8_t rtc_get_minute()
{
	i2c_start(MCP_write);
	i2c_write(0x01);
	i2c_stop();
	
	i2c_start(MCP_read);
	uint8_t minute_data = i2c_read_nack();
	i2c_stop();

	return rtc_binary_to_decimal(minute_data);
}

/**
 * Set a minute register value with given decimal value.
 *
 * @param m is a decimal value for minute.
 */
void rtc_set_minute(uint8_t m)
{
	m = rtc_decimal_to_binary(m);
	i2c_start(MCP_write);
	i2c_write(0x01);
	i2c_write(m);
	i2c_stop();
}

/**
 * Read a data in hour register in decimal format.
 */
uint8_t rtc_get_hour()
{
	i2c_start(MCP_write);
	i2c_write(0x02);
	i2c_stop();

	i2c_start(MCP_read);
	uint8_t hour_data = i2c_read_nack();
	hour_data &= 0x3F;//Only the bottom 6 bits
	i2c_stop();

	return rtc_binary_to_decimal(hour_data);
}

/**
 * Set a hour register value with given decimal value.
 *
 * @param h is a decimal value for hour.
 */
void rtc_set_hour(uint8_t h)
{
	h = rtc_decimal_to_binary(h);
	i2c_start(MCP_write);
	i2c_write(0x02);
	i2c_write(h);
	i2c_stop();
}

/**
 * Set a hour register value with given decimal value.
 *
 * @param h is a decimal value for hour.
 */
void rtc_set_year(uint8_t h)
{
	h = rtc_decimal_to_binary(h);
	i2c_start(MCP_write);
	i2c_write(0x06);
	i2c_write(h);
	i2c_stop();
}

/**
 * Set a hour register value with given decimal value.
 *
 * @param h is a decimal value for hour.
 */
void rtc_set_month(uint8_t h)
{
	h = rtc_decimal_to_binary(h);
	i2c_start(MCP_write);
	i2c_write(0x05);
	i2c_write(h);
	i2c_stop();
}

/**
 * Set a hour register value with given decimal value.
 *
 * @param h is a decimal value for hour.
 */
void rtc_set_date(uint8_t h)
{
	h = rtc_decimal_to_binary(h);
	i2c_start(MCP_write);
	i2c_write(0x04);
	i2c_write(h);
	i2c_stop();
}


/**
 * Convert given binary value to decimal value.
 *
 * @param binary is a value to convert into decimal
 */
uint8_t rtc_binary_to_decimal(uint8_t binary)
{
	uint8_t decimal;
	decimal = ((binary&0xf0)/16)*10;
	decimal += binary%16;
	return decimal;
}

/**
 * Convert given decimal value to binary value.
 *
 * @param decimal is a value to convert into binary
 */
uint8_t rtc_decimal_to_binary(uint8_t decimal)
{
	uint8_t binary;
	binary = (decimal/10)<<4 | (decimal%10);

	return binary;
}

uint8_t reg_read(uint8_t address)
{
	i2c_start(MCP_write);
	i2c_write(address);
	i2c_stop();

	i2c_start(MCP_read);
	//read a transmitted data when NACK transmitted.
	uint8_t data = i2c_read_nack();
	i2c_stop();
	return data;
}

void reg_write(uint8_t address, uint8_t data)
{
	//Start the i2c WRITING communication
	i2c_start(MCP_write);
	//point a register for a second data
	i2c_write(address);
	//write a given value on second register and start the clock (BIT 7)
	i2c_write(data);
	//stop the i2c communication
	i2c_stop();
}

