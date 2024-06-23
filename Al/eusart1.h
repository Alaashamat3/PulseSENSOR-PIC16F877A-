#ifndef EUSART1_H
#define EUSART1_H

#include <xc.h>
#include <stdint.h>

void EUSART1_Initialize(void);
uint8_t EUSART1_Read(void);
void EUSART1_Write(uint8_t);
void put_string(const unsigned char *);
void UART_RxString(char *);

#endif // EUSART1_H
