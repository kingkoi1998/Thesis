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


#include "I2C_master.h"
#include "LCD_driver.h"
#include "RTC.h"
#include "mode.h"
#define RGB_ADDR 0xC0
#define LCD_ADDR 0x7C
#define HOME 1
#define STARTPUMP 2
#define CLEAN 3
#define SETTINGS 4
#define OPTIONS 5
#define PUMP 6
#define STARTCLEAN 7
#define INT2CHAR 48

static uint8_t minutes = 100;
static uint8_t hours = 100;
static uint8_t last_minute = 0;
static uint8_t last_hour = 0;

volatile int button[3] = {0, 0, 0};
	
void set_message(char pass[16], int message) {
	if (message == 1) {//Update message1
		for(int i = 0; i < 16; ++i) {
			message1[i] = pass[i];
		}
	}
	if (message == 2) {//Update message2
		for(int i = 0; i < 16; ++i) {
			message2[i] = pass[i];
		}
	}
}

void check_button_updates(void) {
	//temp disable interrupts
	cli();
	if(button[0] == 1) {//Select Pressed
		toggle_select = true;
		button[0] = 0;
	}
	if(button[1] == 1) {//Right Pressed
		toggle_right = true;
		button[1] = 0;
	}
	if(button[2] == 1) {//Left Pressed
		toggle_left = true;
		button[2] = 0;
	}
	if((toggle_left == true || toggle_right == true || toggle_select == true) && (minutes - last_minute) > 1) {
		last_minute = minutes;
		set_backlight_white(0x0F);
		toggle_left = false;
		toggle_right = false;
		toggle_select = false;
	}
	sei();
}

void init_hardware(void) {
	DDRB &= ~((1 << DDB0) |(1 << DDB1) | (1 << DDB2)); // Clear the PB0, PB1, PB2 pin
	// PB0,PB1,PB2 (PCINT0, PCINT1, PCINT2 pin) are now inputs

	PORTB |= ((1 << PORTB0) |(1 << PORTB1) | (1 << PORTB2)); // turn On the Pull-up
	// PB0, PB1 and PB2 are now inputs with pull-up enabled
		
	PCICR |= (1 << PCIE0);     // set PCIE0 to enable PCMSK0 scan
	PCMSK0 |= ((1 << PCINT0) | (1<<PCINT1) | (1<<PCINT2));   // set PCINT0 to trigger an interrupt on state change
	DDRD = 0xFF;
	PORTD = 0x00;
	
	DDRD |= (1 << DDD3);
	// PD3 is now an output

	OCR2B = 128;
	// set PWM for 50% duty cycle
	TCCR2A |= (1 << COM2B1);
	// set none-inverting mode
	TCCR2A |= (1 << WGM21) | (1 << WGM20);
	// set fast PWM Mode
}

void update_screen_time(void) {
	
	if(hours < 12) {//AM
		if(hours / 10) {//Between 10-12
			home_message[0] = '1';
			home_message[1] = (hours % 10) + INT2CHAR;
		}
		else{
			home_message[0] = ' ';
			home_message[1] = hours + INT2CHAR;
		}
		home_message[5] = 'A';
		home_message[6] = 'M';
	}
	else {//PM
		if(hours == 12) {
			home_message[0] = '1';
			home_message[1] = '2';
		}
		else if((hours - 12) / 10) {//Between 10-12
			home_message[0] = '1';
			home_message[1] = ((hours-12) % 10) + INT2CHAR;
		}
		else{
			home_message[0] = ' ';
			home_message[1] = (hours - 12) + INT2CHAR;
		}
		home_message[5] = 'P';
		home_message[6] = 'M';
	}

	home_message[3] = (minutes / 10) + INT2CHAR;
	home_message[4] = (minutes % 10) + INT2CHAR;
	alarm_minutes -= 1;
	if(alarm_minutes == -1) {
		alarm_hours -= 1;
		alarm_minutes = 59;
	}
	if(alarm_hours == 0 && alarm_minutes == 0) {
		char Mes1[16] = " EMPTY BLADDER! ";
		set_message(Mes1, 1);
		char blank[16] = "Dismiss>        ";
		set_message(blank, 2);
		update_LCD();
		int i = 0;
		while(toggle_select == false) {
			if(i%2 == 0) {
				TCCR2B = (1<<CS20);
			} else {
				TCCR2B = (0<<CS20);
			}
			_delay_ms(50);
			++i;
			if(button[0] == 1) {//Select Pressed
				toggle_select = true;
				button[0] = 0;
			}
		}
		alarm_hours = eeprom_read_byte((uint8_t*)2);
		alarm_minutes = eeprom_read_byte((uint8_t*)3);
		TCCR2B = (0<<CS20);
		toggle_select = false;
		char home_bottom[16] = "MODE>  SETTINGS ";
		cursor_position = 1;
		set_message(home_bottom,2);
		mode = HOME;
	}
	//Write new time to EEPROM
	eeprom_write_byte((uint8_t*)0, alarm_hours);
	eeprom_write_byte((uint8_t*)1, alarm_minutes);
	eeprom_write_byte((uint8_t*)4, hours);//Save the new time to eeprom
	eeprom_write_byte((uint8_t*)5, minutes);//Save the new time to eeprom
	
	home_message[11] = (alarm_hours/10) + INT2CHAR;
	home_message[12] = (alarm_hours%10) + INT2CHAR;
	home_message[14] = (alarm_minutes/10) + INT2CHAR;
	home_message[15] = (alarm_minutes%10) + INT2CHAR;
	set_message(home_message,1);
	update_LCD();
}

 
void run_mode_update(void) {
	if (mode == HOME) {
		run_home();
	} else if (mode == STARTPUMP) {
		start_pump();
	} else if(mode == STARTCLEAN) {
		start_clean();
	} else if (mode == SETTINGS) {
		run_settings();
	} else if (mode == OPTIONS) {
		run_options();
	} else if(mode == PUMP) {
		run_pump();
	} else if (mode == CLEAN) {
		run_clean();
	} else {
		update_LCD();
		_delay_ms(400);
		mode = HOME;
	}
	
}

void update_time(void) {
	if(hours == rtc_get_hour() && minutes == rtc_get_minute()) {
		time_update = false;
	} else {
		hours = rtc_get_hour();
		minutes = rtc_get_minute();
		time_update = true;
	}
	
}

void screen_saver(void) {
	if(minutes - last_minute > 1 && toggle_left == false && toggle_right == false && toggle_select == false) {
		set_backlight_white(0x00);
	}
}

void init_variables(void) {
	uint8_t sec = rtc_get_second();
	hours = 0;
	minutes = 0;
	rtc_set_second(sec);
	last_hour = rtc_get_hour();
	last_minute = rtc_get_minute();
	alarm_hours = eeprom_read_byte((uint8_t*)0);//Count down
	alarm_minutes = eeprom_read_byte((uint8_t*)1);//Count Down
	uint8_t fail_hours = eeprom_read_byte((uint8_t*)4);//Failed Power
	uint8_t fail_minutes = eeprom_read_byte((uint8_t*)5);//Failed Power
	uint8_t hours_change = last_hour - fail_hours;
	uint8_t minutes_change = last_minute - fail_minutes;
	
	alarm_hours = alarm_hours - hours_change;
	alarm_minutes = alarm_minutes - minutes_change + 1;
	if(alarm_hours < 0 && alarm_minutes < 0) {
		alarm_minutes = eeprom_read_byte((uint8_t*)3);
		alarm_hours = eeprom_read_byte((uint8_t*)2);
	}
	eeprom_write_byte((uint8_t*)0, alarm_hours);
	eeprom_write_byte((uint8_t*)1, alarm_minutes);
}

int main(void) {
	_delay_ms(50);//Power on Delay
	init_hardware();
	i2c_init();
	init_rtc();
	//rtc_set_hour(17);
	//rtc_set_minute(11);
	init_variables();
	setup_LCD();
	mode = HOME;
	cursor_position = 1;
	sei();// turn on interrupts

	while(1) {
		check_button_updates();//Check if buttons have been pressed
		update_time();
		screen_saver();
		run_mode_update();
	}
}

ISR (PCINT0_vect)
{	
	if(!(PINB & (1 << PINB0)))
	{
		_delay_ms(20);
		if(!(PINB & (1 << PINB0))) {
			button[0] = 1;
		}
	}
	if(!(PINB & (1 << PINB1)))
	{
		_delay_ms(20);
		if(!(PINB & (1 << PINB1))) {
			button[1] = 1;
		}
	}
	if(!(PINB & (1 << PINB2)))
	{
		_delay_ms(20);
		if(!(PINB & (1 << PINB2))) {
			button[2] = 1;
		}
	}	
}