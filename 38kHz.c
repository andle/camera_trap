// Make sure to save new files in Git folder:
//      C:\Users\McGoo\Documents\GitHub\camera_trap

// CONFIG
#pragma config FOSC = INTOSCIO // Oscillator Selection bits (INTOSC oscillator: IO function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = ON       // MCLR Pin Function Select bit (MCLR pin function is MCLR)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Detect (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)

#include <xc.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <pic12f683.h>
#include <stdint.h>

// GPIO assignment
#define modulate GPIObits.GP2 // modualted output to transmitter

#define NOP4 asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define NOP3 asm("nop"); asm("nop");asm("nop");
#define NOP2 asm("nop"); asm("nop");

void init_ports(void)
{
    ANSELbits.ANS = 0b0000; // 0=digital, 1=analog, does not affect digital output
    TRISIObits.TRISIO2 = 0;
    GPIO = 0; // clear all IO bits
}

void init_osc(void)
{ //Internal 8MHz osc
    OSCCONbits.IRCF = 0b110; //0b000 = 31 kHz, 0b110 = 4MHz, 0b111 = 8MHz
}

int main(void)
{
    init_ports();
    init_osc();

    while(1)// main loop
    {
            modulate = 1;
            NOP4; NOP4; NOP2;
            modulate = 0;
            NOP4; NOP4; NOP2;
    }

    return (EXIT_SUCCESS);
}



