// Make sure to save new files in Git folder:
//                         C:\Users\McGoo\Documents\GitHub\camera_trap

// PIC12F683 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG
#pragma config FOSC = INTOSCIO // Oscillator Selection bits (INTOSC oscillator: CLKOUT function on RA4/OSC2/CLKOUT pin, I/O function on RA5/OSC1/CLKIN)
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
#include <stdint.h>

#define LED GPIObits.GP4
#define NOP4	asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define NOP3	asm("nop"); asm("nop");asm("nop");
#define NOP2	asm("nop"); asm("nop");

void init_ports(void)
{
    //ANSELbits.ANS = 0b0000; // 0=digital, 1=analog, does not affect digital output
    TRISIO = 0b000100; // IO settings: bit 2 input for PWM initialization
    GPIO = 0; // clear all IO bits
}

void init_osc(void)
{ //Internal 8MHz osc
    OSCCONbits.IRCF = 0b110; //0b000 = 31 kHz, 0b110 = 4MHz, 0b111 = 8MHz
}

void TMR2_init(void) // does NOT increment in sleep mode
{// initialize and start TMR2, currently for PWM
    INTCONbits.GIE = 1; // global interrupt enable
    INTCONbits.PEIE = 1; // peripheral interrupt enable
    PIE1bits.TMR2IE = 1; // Enable TMR2 interrupt
    PIR1bits.TMR2IF = 0; // clear interrupt flag

    //TMR2 = 0; // set TMR2 to 0
    T2CON = 0; // reset TMR2 config
    //T2CONbits.TOUTPS = 0b0; //prescale: 0b1111 = 16
    //T2CONbits.T2CKPS = 0b0; //postscale: 0b11 = 16
    PR2 = 51; // TMR2 period //Used in PWM for period
    CCP1CON = 0; // reset CCP module
    CCP1CONbits.CCP1M = 0b1100; //110x = PWM mode, active high, 111x = PWM mode active low
    CCP1CONbits.DC1B = 00; // 2 lsb of 8 for PWM duty cycle
    CCPR1L = 0b00011011; // CCP register low: 6msb of 8 for PWM duty cycle
    CCPR1H = 0b00000000; // CCP register high //
    T2CONbits.TMR2ON = 1; // TMR2 on
    while(TMR2 !=0); // wait a cycle to start PWM
    TRISIObits.TRISIO2 = 0; // enable PWM output
}

void TMR1_init(void) // using 31 kHz clock
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
{// initialize and start TMR0,
    //INTCONbits.GIE = 1; // global interrupt enable  ////////// done int TMR2 _init
    //INTCONbits.PEIE = 1; // peripheral interrupt enable ////////// done int TMR2 _init
    //PIE1bits.TMR0IE = 0; // Enable TMR0 interrupt
    //INTCONbits.TMR0IF = 0; // clear interrupt flag

    OPTION_REG = 0; // TMR0 config
    OPTION_REGbits.T0CS = 0; // Internal instruction cycle clock (FOSC/4)
    OPTION_REGbits.PSA = 0; // prescale assigned to TMR0
    OPTION_REGbits.PS = 0b111; // prescaler: if assigned to TMR0: 0b000 = 1:2, 0b111 = 1:256,
    /*In order to have a 1:1 prescaler value for the Timer0
    module, the prescaler must be assigned to the WDT
    module.*/
    T2CONbits.TMR2ON = 1; // TMR2 on
}

//bool sw; // for LED state memory
//void interrupt ISR()
//{// ISR
//    if(PIR1bits.TMR2IF)
//    {
//        TMR2 = 0; // clear TMR2
//        PIR1bits.TMR2IF = 0; // clear IF
//        sw = !sw; // switch LED
//        LED = sw; // set LED
//    }
//
//    if(PIR1bits.CCP1IF)
//    {
//        //  ,TMR1 reset by CCP mode
//        PIR1bits.CCP1IF = 0;
//        sw = !sw; // switch LED
//        LED = sw; // set LED
//    }
//
//    if(INTCONbits.T0IF)
//    {
//        // TMR0 interrupts only on rollover
//        INTCONbits.T0IF = 0; // clear IF
//        sw = !sw; // switch LED
//        LED = sw; // set LED
//    }
//}

#define SendIRPulse(x) SendIRPulseCycles(x/25);
void SendIRPulseCycles(char cycles)
{
	while (cycles--)		//this loop is exactly 25us - of approximately 50% dutycycle
	{
		LED = 1;
		NOP4; NOP4; NOP3;
		LED = 0;
		NOP3;
	}
}

#define waitExactUs(x) waitExactUsHex(x/256, (x%256)/5);
void waitExactUsHex(char hByte, char lByte)
{
    char i;
    while (lByte--) { continue; }
    while (hByte--)
    {
        i = 35;
        while (i--) { continue; }
    }
}

void trigger()
{
	SendIRPulse(2000);
	waitExactUs(27800);
	SendIRPulse(500);
	waitExactUs(1500);
	SendIRPulse(500);
	waitExactUs(3500);
	SendIRPulse(500);

        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(13000);
        
        SendIRPulse(2000);
	waitExactUs(27800);
	SendIRPulse(500);
	waitExactUs(1500);
	SendIRPulse(500);
	waitExactUs(3500);
	SendIRPulse(500);

}

int main(void)
{
    init_ports();
    init_osc();
    //TMR2_init();   /////Choose TMR init based on interrupt scheme, for now
    //TMR1_init();
    //TMR0_init();

    while(1)
    {
        trigger();
        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);

        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);

        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);

        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);

        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);
        waitExactUs(25000);

    }

    return (EXIT_SUCCESS);
}


