#include <xc.h>
#include "eusart1.h"

void EUSART1_Initialize(void) {
    TRISC = 0x80; // Configure Rx pin as input and Tx as output  
    TXSTA = 0x20; // Asynchronous mode, 8-bit data & enable transmitter
    RCSTA = 0x90; // 8-bit continuous receive enable
    SPBRG = 31; // 9600 Baud rate at 20MHz
    RCSTAbits.CREN = 1;
}

uint8_t EUSART1_Read(void) {
    while (!PIR1bits.RCIF);
    return RCREG;
}

void EUSART1_Write(uint8_t txData) {
    while (!PIR1bits.TXIF);
    TXREG = txData;
}

void put_string(const unsigned char *str) {
    while (*str) {
        EUSART1_Write(*str++);
    }
}

void UART_RxString(char *string_ptr) {
    char ch;
    while (1) {
        ch = EUSART1_Read();
        if ((ch == '\r') || (ch == '\n')) {
            *string_ptr = 0;
            break;
        }
        *string_ptr++ = ch;
    }
}