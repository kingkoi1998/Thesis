#ifndef LCD_DRIVER_H_
#define LCD_DRIVER_H_
#define RGB_ADDR 0xC0
#define LCD_ADDR 0x7C

extern uint8_t message1[16];
extern uint8_t message2[16];
extern uint8_t home_message[16];

void set_backlight_white(uint8_t input[1]);
void setup_LCD(void);
void update_LCD(void);

#endif /* LCD_DRIVER_H_ */