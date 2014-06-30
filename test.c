#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTOSCIO  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
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

//#define LED GPIObits.GP4

void init_ports(void)
{
    TRISIO = 0;
    GPIO = 0;
    //LATGPIO = 0;
}

void init_osc(void)
{ //Internal 8MHz osc
    OSCCONbits.IRCF = 0b110; //000 = 31 kHz, 110 = 4MHz
}

//void delay_ms(unsigned int cnt)// soft delay
//{
//    while(cnt != 0)
//    {// 4 Tcy 1st, 6 Tcy subsequent
//        for(int i = 82; i != 0; i--)
//        {// 8 Tcy 1st, 11 Tcy sub
//            asm("NOP");     // 1 Tcy
//        }
//        cnt--;             // 6 Tcy
//    }
//}

void TMR2_init(void)
{
    INTCONbits.PEIE = 1;
    PIE1bits.TMR2IE = 1; // Enable TMR2 inter
    //INTE = 1;
    //GIE = 1;
    PIR1bits.TMR2IF = 0;
    TMR2 = 0; // set TMR2 to 0
    T2CON = 0; // reset TMR2 config
    //T2CONbits.TOUTPS = 0b1111; //prescale 16
    //T2CONbits.T2CKPS = 0b11; //postscale 16
    PR2 = 121; // TMR2 period
    T2CONbits.TMR2ON = 1; // TMR2 on
}

void interrupt ISR()
{// ISR
        TMR2 = 0;
        PIR1bits.TMR2IF = 0;
        GPIObits.GP4 = 1;
}

int main(void)
{
    init_ports();
    init_osc();
    TMR2_init();
    while(1);
//    {
//        GPIO = (1<<4);
//        delay_ms(500);
//
//        GPIO = 0;
//        delay_ms(500);
//    }

    return (EXIT_SUCCESS);
}

