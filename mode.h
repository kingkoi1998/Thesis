/*
 * mode.h
 *
 * Created: 7/09/2019 7:44:12 PM
 *  Author: ethan
 */ 


#ifndef MODE_H_
#define MODE_H_
#define HOME 1
#define STARTPUMP 2
#define CLEAN 3
#define SETTINGS 4
#define OPTIONS 5
#define PUMP 6
#define STARTCLEAN 7
#define INT2CHAR 48

extern bool toggle_left;
extern bool toggle_right;
extern bool toggle_select;
extern bool time_update;
extern int cursor_position;
extern int mode;
extern uint8_t alarm_hours;
extern uint8_t alarm_minutes;

void run_home(void);
void run_options(void);
void start_pump(void);
void start_clean(void);
void run_pump(void);
void handle_alarm_setting(void);
void run_settings(void);
void run_clean(void);


#endif /* MODE_H_ */