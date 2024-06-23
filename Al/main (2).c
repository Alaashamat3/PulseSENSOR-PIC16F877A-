
// Alaa Shamat / Saleem Abu Zaid
// Medipol university
// Pulse sensor pic16f877A

#include <xc.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "adc.h"
#include "eusart1.h"
#include "lcd.h"
#include "tmr0.h"

// Configuration bits
#pragma config FOSC = HS
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config BOREN = ON
#pragma config LVP = OFF
#pragma config CPD = OFF
#pragma config WRT = OFF
#pragma config CP = OFF

#define _XTAL_FREQ 20000000

#define READ_ADC  1
#define CALCULATE_HEART_BEAT 2
#define SHOW_HEART_BEAT 3
#define IDLE 0

volatile int rate[10];
volatile unsigned long sampleCounter = 0;
volatile unsigned long lastBeatTime = 0;
volatile int P = 512;
volatile int T = 512;
volatile int thresh = 530;
volatile int amp = 0;
volatile bool firstBeat = true;
volatile bool secondBeat = false;
volatile int BPM;
volatile int Signal;
volatile int IBI = 600;
volatile bool Pulse = false;
volatile bool QS = false;

int main_state = -1;
int adc_value = 0;

void system_init(void);
void calculate_heart_beat(int adc_value);

void calculate_heart_beat(int adc_value) {
    Signal = adc_value;
    sampleCounter += 2;
    int N = sampleCounter - lastBeatTime;

    if (Signal < thresh && N > (IBI / 5) * 3) {
        if (Signal < T) {
            T = Signal;
        }
    }

    if (Signal > thresh && Signal > P) {
        P = Signal;
    }

    if (N > 250) {
        if (Signal > thresh && !Pulse && N > (IBI / 5) * 3) {
            Pulse = true;
            IBI = sampleCounter - lastBeatTime;
            lastBeatTime = sampleCounter;

            if (secondBeat) {
                secondBeat = false;
                for (int i = 0; i <= 9; i++) {
                    rate[i] = IBI;
                }
            }

            if (firstBeat) {
                firstBeat = false;
                secondBeat = true;
                return;
            }

            uint16_t runningTotal = 0;
            for (int i = 0; i <= 8; i++) {
                rate[i] = rate[i + 1];
                runningTotal += rate[i];
            }

            rate[9] = IBI;
            runningTotal += rate[9];
            runningTotal /= 10;
            BPM = 60000 / runningTotal;
            QS = true;
        }
    }

    if (Signal < thresh && Pulse) {
        Pulse = false;
        amp = P - T;
        thresh = amp / 2 + T;
        P = thresh;
        T = thresh;
    }

    if (N > 2500) {
        thresh = 530;
        P = 512;
        T = 512;
        lastBeatTime = sampleCounter;
        firstBeat = true;
        secondBeat = false;
    }
}

void main() {
    system_init();
    main_state = READ_ADC;

    while (1) {
        switch (main_state) {
            case READ_ADC:
                adc_value = ADC_Read(0);
                main_state = CALCULATE_HEART_BEAT;
                break;
            case CALCULATE_HEART_BEAT:
                calculate_heart_beat(adc_value);
                main_state = SHOW_HEART_BEAT;
                break;
            case SHOW_HEART_BEAT:
                if (QS) {
                    QS = false;
                    lcd_com(0x80);
                    lcd_puts("Your heart beat");
                    lcd_com(0xC0);
                    lcd_puts("is: ");
                    lcd_print_number(BPM);
                    lcd_puts(" BPM");
                    __delay_ms(3000); // Delay to show the heart beat
                    lcd_com(0x01); // Clear display
                    lcd_com(0x80); // Move cursor to the first line
                    lcd_puts("Thank You!!");
                }
                main_state = IDLE;
                break;
            case IDLE:
                break;
        }
    }
}

void system_init(void) {
    TRISB = 0x00; // Set PORTB as output for LCD
    lcd_init(); // This will initialize the lcd
    TMR0_Initialize();
    TMR0_StartTimer();
    INTCONbits.GIE = 1; // Enable Global Interrupt
    INTCONbits.PEIE = 1; // Enable Peripheral Interrupt
    ADC_Init();
}

void timer_isr() {
    main_state = READ_ADC;
}

void __interrupt() INTERRUPT_InterruptManager(void) {
    if (INTCONbits.TMR0IE && INTCONbits.TMR0IF) {
        TMR0_ISR();
    }
}