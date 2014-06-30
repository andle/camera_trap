// Make sure to save new files in Git folder:
//                         C:\Users\McGoo\Documents\GitHub\camera_trap

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
    INTCONbits.GIE = 1; // global interrupt enable
    INTCONbits.PEIE = 1; // peripheral interrupt enable
    PIE1bits.TMR2IE = 1; // Enable TMR2 interrupt
    PIR1bits.TMR2IF = 0; // clear interrupt flag

    TMR2 = 0; // set TMR2 to 0
    T2CON = 0; // reset TMR2 config
    T2CONbits.TOUTPS = 0b1111; //prescale 16
    T2CONbits.T2CKPS = 0b11; //postscale 16
    PR2 = 30; // TMR2 period: 32kHz/4/16/16 = 30.27,
    T2CONbits.TMR2ON = 1; // TMR2 on
}

void TMR1_init(void)
{// initialize and start TMR1, using CCP compare mode with Special Event Trigger
    INTCONbits.GIE = 1; // global interrupt enable
    INTCONbits.PEIE = 1; // peripheral interrupt enable
    PIE1bits.CCP1IE = 1; // Enable CCP interrupt
    //PIE1bits.T1IE = 1;
    // Not using TMR1 interrupt, ony set on rollover
    
    PIR1bits.CCP1IF = 0; // clear CCP interrupt flag
    // If using INTOSC and CLKOUT (see config) can use built in LP 32.768 osc, usage below
    //T1CONbits.T1OSCEN = 1; // LP 32.768 Hz osc for TMR1

    TMR1 = 0; // reset TMR1
    T1CON = 0; // reset TMR2 config
    //T1CONbits.T1CKPS = 0; //prescale
    CCP1CON = 0; // reset CCP module
    CCP1CONbits.CCP1M = 0b1011; /*Compare mode, trigger special event
    * (CCP1IF bit is set and TMR1 is reset. CCP1 pin is unaffected.)*/
    CCPR1L = 0b01000110; // CCP register low
    CCPR1H = 0b11110; // CCP register high // these two function as period register for special event trigger
    // 31kHz/4 = 7750 Hz = 11110 01000110

    T1CONbits.TMR1ON = 1; // TMR1 on
}

void TMR0_init(void)
{// initialize and start TMR0, on/off 15 times a second if Fosc = 31 kHz
    INTCONbits.GIE = 1; // global interrupt enable
    INTCONbits.PEIE = 1; // peripheral interrupt enable
    PIE1bits.TMR2IE = 1; // Enable TMR0 interrupt

    PIR1bits.TMR2IF = 0; // clear interrupt flag
    TMR0 = 0; // set TMR2 to 0
    OPTION_REGbits.T0CS = 0; // reset TMR2 config
    OPTION_REGbits.PSA = 0; // prescale assigned to TMR0
    OPTION_REGbits.PS = 0b111; // prescaler
    T2CONbits.TMR2ON = 1; // TMR2 on
}

bool sw; // for LED state memory
void interrupt ISR()
{// ISR
    if(PIR1bits.TMR2IF)
    {
        TMR2 = 0; // clear TMR2
        PIR1bits.TMR2IF = 0; // clear IF
        sw = !sw; // switch LED
        LED = sw; // set LED
    }
    
    if(PIR1bits.CCP1IF)
    {
        // TMR1 reset by CCP mode
        PIR1bits.CCP1IF = 0;
        sw = !sw; // switch LED
        LED = sw; // set LED
    }

    if(INTCONbits.T0IF)
    {
        // TMR0 interrupts only on rollover
        INTCONbits.T0IF = 0; // clear IF
        sw = !sw; // switch LED
        LED = sw; // set LED
    }
}

int main(void)
{
    init_ports();
    init_osc();
    //TMR2_init();   /////Choose TMR init based on interrupt scheme, for now
    //TMR1_init();
    TMR0_init();
    while(1);

    return (EXIT_SUCCESS);
}


