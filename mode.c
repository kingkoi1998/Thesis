/*
 * mode.c
 *
 * Created: 7/09/2019 7:44:00 PM
 *  Author: ethan
 */ 

#ifndef F_CPU
#define F_CPU 8000000L
#endif

#include <avr/io.h>
#include <util/delay.h>
#include <math.h>
#include <stdlib.h>
#include <avr/interrupt.h>
#include <stdbool.h>
#include <avr/eeprom.h>


#include "mode.h"
#include "I2C_master.h"
#include "LCD_driver.h"
#include "RTC.h"


bool toggle_left = false;
bool toggle_right = false;
bool toggle_select = false;
bool time_update = false;

int cursor_position;
int mode;
int setting_param;
int setting_alarm;
uint8_t alarm_hours;
uint8_t alarm_minutes;

void run_home(void) {
	if(toggle_left == true && cursor_position == 2) {
		message2[4] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1) {
		message2[15] = 0x3E;
		message2[4] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1) {//Go to options
		mode = OPTIONS;
		char pasMes[16] = "PUMP>     CLEAN ";
		set_message(pasMes, 2);
		char pass[16] = "CHOOSE MODE?    ";
		set_message(pass, 1);
		cursor_position = 1;
		update_LCD();
	}
	if(time_update == true) {//If the time on screen needs updating
		update_screen_time();
	}
	if(toggle_select == true && cursor_position == 2) {
		mode = SETTINGS;
		char pasMes[16] = "Test Alarm?     ";
		set_message(pasMes, 1);
		char pass[16] = "Yes>       Next ";
		set_message(pass, 2);
		cursor_position = 1;
		setting_param = 1;
		update_LCD();
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}

void run_options(void)
{
	if(toggle_left == true && cursor_position == 2) {
		message2[4] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1) {
		message2[15] = 0x3E;
		message2[4] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1) {//Go to options
		mode = STARTPUMP;
		char pasMes[16] = "ENTER PUMP MODE?";
		set_message(pasMes, 1);
		char pass[16] = "YES>         NO ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
	}
	if(toggle_select == true && cursor_position == 2) {//Go to options
		mode = STARTCLEAN;
		char pasMes[16] = "CLEANING MODE?  ";
		set_message(pasMes, 1);
		char pass[16] = "YES>         NO ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}

void start_pump(void) {
	if(toggle_left == true && cursor_position == 2) {
		message2[3] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1) {
		message2[15] = 0x3E;
		message2[3] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1) {//Go to pump home
		mode = PUMP;
		char pasMes[16] = "                ";
		set_message(pasMes, 1);
		char pass[16] = "PUMP>      HOME ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
	}
	if(toggle_select == true && cursor_position == 2) {//Go to pump home
		mode = HOME;
		set_message(home_message, 1);
		char pass1[16] = "MODE>  SETTINGS ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}

void start_clean(void) {
	if(toggle_left == true && cursor_position == 2) {
		message2[3] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1) {
		message2[15] = 0x3E;
		message2[3] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1) {
		//Enter to cleaning mode
		mode = CLEAN;
		char pasMes[16] = "                ";
		set_message(pasMes, 1);
		char pass[16] = "CLEAN>     HOME ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
	}
	if(toggle_select == true && cursor_position == 2) {//Go to Home page
		mode = HOME;
		set_message(home_message, 1);
		char pass1[16] = "MODE>  SETTINGS ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}

void run_pump(void) {
	if(toggle_left == true && cursor_position == 2) {
		message2[4] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1) {
		message2[15] = 0x3E;
		message2[4] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1) {//Turn on PUMP
		char pasMes[16] = "PUMPING         ";
		set_message(pasMes, 1);
		char pass[16] = "                ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
		PORTD = 0b10100000;
		for(int i = 7; i < 16; ++i) {//Run for 5 seconds
			_delay_ms(55);
			message1[i] = '>';//Add progress update
			update_LCD();
		}
		PORTD = 0x00;
		char pasMes1[16] = "                ";
		set_message(pasMes1, 1);
		char pass1[16] = "PUMP>      HOME ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
	}
	if(toggle_select == true && cursor_position == 2) {//Go to Home page
		mode = HOME;
		set_message(home_message, 1);
		char pass1[16] = "MODE>  SETTINGS ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
		setting_alarm = 1;
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}

void handle_alarm_setting(void) {
	if(toggle_select == true) {
		if(cursor_position == 1) {
			mode = HOME;
			set_message(home_message, 1);
			char pass1[16] = "MODE>  SETTINGS ";
			set_message(pass1, 2);
			cursor_position = 1;
			home_message[11] = (alarm_hours/10) + INT2CHAR;
			home_message[12] = (alarm_hours%10) + INT2CHAR;
			home_message[14] = (alarm_minutes/10) + INT2CHAR;
			home_message[15] = (alarm_minutes%10) + INT2CHAR;
			set_message(home_message,1);
			
			update_LCD();
			} else {//Change interval time
			int pos = cursor_position + 9;
			switch (cursor_position) {
				case 2:
				if(alarm_hours > 19) {//contains a 2
					alarm_hours = alarm_hours - 20;
					message1[pos] = '0';
					} else {
					if(alarm_hours > 9) {//contains a 1
						alarm_hours += 10;
						message1[pos] = '2';
					}
					if(alarm_hours < 10) {
						alarm_hours += 10;
						message1[pos] = '1';
					}
				}
				break;
				case 3:
				if(alarm_hours%10 == 9) {
					alarm_hours -= 9;
					message1[pos] = '0';
				} else {
					alarm_hours += 1;
					message1[pos] = (alarm_hours % 10) + INT2CHAR;
				}
				break;
				case 4:
				if(alarm_minutes > 49) {
					alarm_minutes = alarm_minutes - 50;
					message1[pos+1] = '0';
					} else {
					alarm_minutes += 10;
					message1[pos+1] = (alarm_minutes / 10) + INT2CHAR;
				}
				break;
				case 5:
				if(alarm_minutes % 10 == 9) {
					alarm_minutes = alarm_minutes - 9;
					message1[pos+1] = '0';
					} else {
					alarm_minutes += 1;
					message1[pos+1] = (alarm_minutes % 10) + INT2CHAR;
				}
				default:
				break;
			}
			update_LCD();
			eeprom_write_byte((uint8_t*)0, alarm_hours);//Count down EEPROM hours
			_delay_ms(3);
			eeprom_write_byte((uint8_t*)1, alarm_minutes);//Count down EEPROM minutes
			_delay_ms(3);
			eeprom_write_byte((uint8_t*)2, alarm_hours);//Interval Hours
			_delay_ms(3);
			eeprom_write_byte((uint8_t*)3, alarm_minutes);//Interval Minutes
			_delay_ms(3);
		}
	}
	if(toggle_right == true) {
		switch (cursor_position) {
			case 1:
			cursor_position = 2;
			message2[11] = '^';
			message2[4] = ' ';
			break;
			case 2:
			cursor_position = 3;
			message2[12] = '^';
			message2[11] = ' ';
			break;
			case 3:
			cursor_position = 4;
			message2[14] = '^';
			message2[12] = ' ';
			break;
			case 4:
			cursor_position = 5;
			message2[15] = '^';
			message2[14] = ' ';
			break;
			case 5:
			cursor_position = 1;
			message2[4] = '>';
			message2[15] = ' ';
			break;
		}
		update_LCD();
	}
	if(toggle_left == true) {
		switch (cursor_position) {
			case 1:
			cursor_position = 5;
			message2[15] = '^';
			message2[4] = ' ';
			break;
			case 2:
			cursor_position = 1;
			message2[4] = '>';
			message2[11] = ' ';
			break;
			case 3:
			cursor_position = 2;
			message2[11] = '^';
			message2[12] = ' ';
			break;
			case 4:
			cursor_position = 3;
			message2[12] = '^';
			message2[14] = ' ';
			break;
			case 5:
			cursor_position = 4;
			message2[14] = '^';
			message2[15] = ' ';
			break;
		}
		update_LCD();
	}
}

void run_settings(void) {
	if(toggle_left == true && cursor_position == 2 && setting_param != 3) {
		message2[3] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1 && setting_param != 3) {
		message2[15] = 0x3E;
		message2[3] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1 && setting_param == 1) {//Test alarm
		char pasMes[16] = "Testing Alarm!  ";
		set_message(pasMes, 1);
		char pass[16] = "                ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
		TCCR2B |= (1<<CS21);
		for(int i = 0; i < 10; ++i) {
			if(i%2 == 0) {
				set_message(pass, 1);
				TCCR2B = (1<<CS20);
			} else {
				set_message(pasMes, 1);
				TCCR2B = (0<<CS20);
			}
			update_LCD();
			_delay_ms(50);
		}
		TCCR2B = (0<<CS21);
		mode = HOME;
		set_message(home_message, 1);
		char pass1[16] = "MODE>  SETTINGS ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
		
	}
	if(toggle_select == true && cursor_position == 2 && setting_param == 1) {//Set the interval alarm
		char pasMes[16] = "Set Alarm?      ";
		set_message(pasMes, 1);
		char pass[16] = "Yes>       Next ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
		setting_param = 2;
		toggle_select = false;
	}
	if(toggle_select == true && cursor_position == 1 && setting_param == 2) {
		char pasMes[16] = "Interval:    :  ";
		set_message(pasMes, 1);
		message1[11] = (alarm_hours/10) + INT2CHAR;
		message1[12] = (alarm_hours%10) + INT2CHAR;
		message1[14] = (alarm_minutes/10) + INT2CHAR;
		message1[15] = (alarm_minutes%10) + INT2CHAR;
		char pass[16] = "Home       ^    ";
		set_message(pass, 2);
		update_LCD();
		setting_param = 3;
		cursor_position = 2;
	}
	if(setting_param == 3) {
		handle_alarm_setting();
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}

void run_clean(void) {
	if(toggle_left == true && cursor_position == 2) {
		message2[5] = 0x3E;
		message2[15] = ' ';
		update_LCD();
		cursor_position = 1;
	}
	if(toggle_right == true && cursor_position == 1) {
		message2[15] = 0x3E;
		message2[5] = ' ';
		update_LCD();
		cursor_position = 2;
	}
	if(toggle_select == true && cursor_position == 1) {//CLEAN CYCLE
		char pasMes[16] = "CLEANING        ";
		set_message(pasMes, 1);
		char pass[16] = "                ";
		set_message(pass, 2);
		cursor_position = 1;
		update_LCD();
		PORTD = 0b10100000;
		for(int i = 8; i < 16; ++i) {//Run for 10 seconds
			_delay_ms(550);
			message1[i] = '>';//Add progress update
			update_LCD();
		}
		PORTD = 0x00;
		mode = HOME;
		set_message(home_message, 1);
		char pass1[16] = "MODE>  SETTINGS ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
	}
	if(toggle_select == true && cursor_position == 2) {//Go to Home page
		mode = HOME;
		set_message(home_message, 1);
		char pass1[16] = "MODE>  SETTINGS ";
		set_message(pass1, 2);
		cursor_position = 1;
		update_LCD();
		setting_alarm = 1;
	}
	toggle_right = false;
	toggle_left = false;
	toggle_select = false;
}