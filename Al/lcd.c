#include <xc.h>
#include "lcd.h"

#define RS PORTBbits.RB0
#define E PORTBbits.RB1
#define D4 PORTBbits.RB2
#define D5 PORTBbits.RB3
#define D6 PORTBbits.RB4
#define D7 PORTBbits.RB5

bit status = 0;
#define lcd_delay 300

#define LCDMaxLines 2
#define LCDMaxChars 16
#define LineOne 0x80
#define LineTwo 0xc0

#define BlankSpace ' '

int num[10];
unsigned char c;

void delay(unsigned int j) {
    unsigned int i = 0;
    for (i = 0; i < j; i++);
}

void lcd_send_nibble(unsigned char nibble) {
    D4 = (nibble >> 0) & 0x01;
    D5 = (nibble >> 1) & 0x01;
    D6 = (nibble >> 2) & 0x01;
    D7 = (nibble >> 3) & 0x01;
    E = 1;
    delay(lcd_delay);
    E = 0;
    delay(lcd_delay);
}

void lcd_com(unsigned char command) {
    RS = 0;
    lcd_send_nibble(command >> 4);
    lcd_send_nibble(command & 0x0F);
}

void lcd_data(unsigned char data) {
    RS = 1;
    lcd_send_nibble(data >> 4);
    lcd_send_nibble(data & 0x0F);
}

void lcd_init(void) {
    delay(lcd_delay);
    RS = 0;
    E = 0;
    delay(lcd_delay);
    lcd_send_nibble(0x3);
    delay(lcd_delay);
    lcd_send_nibble(0x3);
    delay(lcd_delay);
    lcd_send_nibble(0x3);
    delay(lcd_delay);
    lcd_send_nibble(0x2);

    lcd_com(0x28); // Function set: 4-bit/2-line
    lcd_com(0x0C); // Display ON; Cursor OFF
    lcd_com(0x06); // Entry mode set
    lcd_com(0x01); // Clear display
    delay(lcd_delay);
}

void lcd_puts(const char *str) {
    unsigned int i = 0;
    while (str[i] != '\0') {
        lcd_data(str[i]);
        i++;
    }
}

void lcd_print_number(unsigned int i) {
    int p;
    int k = 0;
    while (i > 0) {
        num[k] = i % 10;
        i = i / 10;
        k++;
    }
    k--;
    for (p = k; p >= 0; p--) {
        c = num[p] + 48;
        lcd_data(c);
    }
}