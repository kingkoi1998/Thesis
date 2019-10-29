
#define MCP_write 0b11011110
#define MCP_read 0b11011111

void init_rtc();

uint8_t rtc_get_second();
void rtc_set_second(uint8_t s);

uint8_t rtc_get_minute();
void rtc_set_minute(uint8_t m);

uint8_t rtc_get_hour();
void rtc_set_hour(uint8_t h);
void rtc_set_date(uint8_t h);
void rtc_set_month(uint8_t h);
void rtc_set_year(uint8_t h);

