#ifndef LCD_H
#define LCD_H

void lcd_init(void);
void lcd_com(unsigned char a);
void lcd_data(unsigned char a);
void lcd_puts(const char *str);
void lcd_print_number(unsigned int i);
void itoa_alt(char* buffer, int num, int base); // Declaration of the renamed function
int abs_alt(int n); // Declaration of the renamed function

#endif /* LCD_H */
