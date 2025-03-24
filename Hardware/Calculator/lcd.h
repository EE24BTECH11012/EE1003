/* lcd.c */
void write_string_to_buffer(const char *src, char *dest, int *index);
char keypad_scan(void);
void lcd_init(void);
void lcd_command(unsigned char cmd);
void lcd_char(unsigned char data);
void lcd_string(const char *str);
void lcd_string_P(const char *str);
void lcd_clear(void);
void keypad_init(void);
