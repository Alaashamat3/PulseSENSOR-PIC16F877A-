#include <xc.h>
#include "tmr0.h"

#define _XTAL_FREQ 20000000

void TMR0_Initialize(void) {
    OPTION_REGbits.T0CS = 0;
    OPTION_REGbits.T0SE = 0;
    OPTION_REGbits.PSA = 0; // prescale use for only Timer0 not for WatchDog_timer
    OPTION_REGbits.PS = 0x07;
    TMR0 = 256 - (_XTAL_FREQ / 4 / 256 / 1000 * 2); // delay for 2ms
    INTCONbits.TMR0IF = 0;
    INTCONbits.TMR0IE = 1; // Enable Timer0 interrupt
}

void TMR0_StartTimer(void) {
    INTCONbits.TMR0IE = 1;
}

void TMR0_StopTimer(void) {
    INTCONbits.TMR0IE = 0;
}

void TMR0_ISR(void) {
    static volatile unsigned int CountCallBack = 0;
    INTCONbits.TMR0IE = 0;
    INTCONbits.TMR0IF = 0;
    TMR0 = 256 - (_XTAL_FREQ / 4 / 256 / 1000 * 2); // delay for 2ms

    if (++CountCallBack >= 10) { // Assuming 2ms ISR call, 10 * 2ms = 20ms
        timer_isr();
        CountCallBack = 0;
    }

    INTCONbits.TMR0IE = 1;
}