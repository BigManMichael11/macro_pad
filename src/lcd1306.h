#ifndef LCD1306_H
#define LCD1306_H
#endif

int clear_screen();
int print_char( const uint8_t col, const uint8_t row, const uint8_t c);
int print_str(const uint8_t col, const uint8_t row, const char *str, const uint8_t length);
uint8_t lcd1306_init();