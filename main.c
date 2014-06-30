// PIC12F683 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTOSCCLK // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pic12f683.h>

#define LED GPIObits.GP0

void init_ports(void)
{
    TRISIO = 0; // set all to outputs
    GPIO = 0; // clear all IO bits
}

void init_osc(void)
{ //Internal 8MHz osc
    OSCCONbits.IRCF = 0b000; //000 = 31 kHz, 110 = 4MHz
}

void TMR2_init(void)
{// initialize and start TMR2
    INTCONbits.PEIE = 1; // peripheral interrupt enable
    PIE1bits.TMR2IE = 1; // Enable TMR2 interrupt
    GIE = 1; // global interrupt enable
    PIR1bits.TMR2IF = 0; // clear interrupt flag
    TMR2 = 0; // set TMR2 to 0
    T2CON = 0; // reset TMR2 config
    T2CONbits.TOUTPS = 0b1111; //prescale 16
    T2CONbits.T2CKPS = 0b11; //postscale 16
    PR2 = 31; // TMR2 period
    T2CONbits.TMR2ON = 1; // TMR2 on
    //LED = 1;
}

bool sw; // for LED state memory
void interrupt ISR()
{// ISR
    TMR2 = 0; // clear TMR2
    PIR1bits.TMR2IF = 0; // clear IF
    //GPIObits.GP0 = 1; // LED
    sw = !sw; // switch LED
    LED = sw; // set LED
}

int main(void)
{
    init_ports();
    init_osc();
    TMR2_init();
    while(1);

    return (EXIT_SUCCESS);
}


