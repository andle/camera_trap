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

// GPIO assignments, double check these
#define IRLED GPIObits.GP0 // output to camera trigger IR transmitter LED
#define indicateDetect GPIObits.GP1 // output to powerOn/detectConfirm indicator LED
#define modulate GPIObits.GP2 // modulated output to detector
//#define indicatePower GPIObits.GP3 // cannot use GPIO3 as output
#define startButton GPIObits.GP4 // input from start button
#define detect GPIObits.GP5 // input signal from detector

#define NOP4 asm("nop"); asm("nop"); asm("nop"); asm("nop");
#define NOP3 asm("nop"); asm("nop");asm("nop");
#define NOP2 asm("nop"); asm("nop");

void init_ports(void)
{
    ANSELbits.ANS = 0b0000; // 0=digital, 1=analog, does not affect digital output
    TRISIObits.TRISIO0 = 0; // ouput
    TRISIObits.TRISIO1 = 0; // ouput
    TRISIObits.TRISIO2 = 0; // ouput
    //TRISIObits.TRISIO3 = 0; // 3 always input
    TRISIObits.TRISIO4 = 1; // input
    TRISIObits.TRISIO5 = 1; // input
    GPIO = 0; // clear all IO bits

    OPTION_REGbits.nGPPU = 0;
    WPUbits.WPU4 = 1; //weak pull up on startButton input
    WPUbits.WPU5 = 1; //weak pull up on detect input    
}

void init_osc(void)
{ //Internal 8MHz osc
    OSCCONbits.IRCF = 0b110; //0b000 = 31 kHz, 0b110 = 4MHz, 0b111 = 8MHz
}

void TMR0_init(void)
{// initialize and start TMR0
    INTCONbits.T0IF = 0; // clear interrupt flag
    INTCONbits.T0IE = 1; // Enable TMR0 interrupt
    INTCONbits.PEIE = 1; // peripheral interrupt enable
    INTCONbits.GIE = 1; // global interrupt enable

    OPTION_REGbits.T0CS = 0; // Internal instruction cycle clock (FOSC/4)
    OPTION_REGbits.PSA = 0; // prescale assigned to TMR0
    OPTION_REGbits.PS = 0b111; // prescaler: if assigned to TMR0: 0b000 = 1:2, 0b111 = 1:256,
}

// IR pulse in microseconds function
#define IRPulse(x) IRPulseCycles(x/25); // change input from microseconds to cycles
void IRPulseCycles(char cycles)
{ // IR pulse function
    while (cycles--)		//this loop is exactly 25us - of approximately 50% dutycycle
    {
        IRLED = 1;
        NOP4; NOP4; NOP3;
        IRLED = 0;
        NOP3;
    }
}

// microsecond delay function
#define delayUs(x) delayUsHex(x/256, (x%256)/5); // change input from microseconds to cycles
void delayUsHex(char hByte, char lByte)
{
    char i;
    while (lByte--) { continue; }
    while (hByte--)
    {
        i = 35; // vary this to tune
        while (i--) { continue; }
    }
}

int sec = 0, min = 0, cnt = 0;
void trigger(void)
{ // camera trigger sequence
    IRPulse(2000);
    delayUs(27800);
    IRPulse(500);
    delayUs(1500);
    IRPulse(500);
    delayUs(3500);
    IRPulse(500);

    delayUs(25000);
    delayUs(25000);
    delayUs(13000);

    IRPulse(2000);
    delayUs(27800);
    IRPulse(500);
    delayUs(1500);
    IRPulse(500);
    delayUs(3500);
    IRPulse(500);

    min = 0; sec = 0; cnt = 0; // reset timeout variables
}

void startup(void)
{ // modulate out to detector
    indicateDetect = 1;
    int j, k;
    bool sw;

    for(j=6; j !=0; j--) // flash indicator for poweron or reset
    {
        sw = !sw;
        indicateDetect = sw;
        for(k=20; k !=0; k--) // turn
            {
                delayUs(25000);
            }
    }
    while(startButton | detect) // wait for start button press and beam detection, both active low
        // start button active low, detector active low (will be low as long as beam is received)
    {
        if(!detect) // if beam reflecting and sensed, active low
        {
            indicateDetect = 1;
        }
        if(detect) // if beam not detected, active low
        {
            indicateDetect = 0;
        }
    }
    indicateDetect = 0;
}

void interrupt ISR()
{
    if(INTCONbits.TMR0IF)
    {
        // TMR0 interrupts only on rollover
        INTCONbits.T0IF = 0; // clear IF
        cnt++;
        if(cnt == 15) //(Finst/TMR0max)/prescale = (1e6/256)/256 = 15.25 per second
        {
            cnt = 0;
            sec++;
        }
        if(sec == 60)
        {
            sec = 0;
            min++;
        }
        if(min == 14)// this logic is a little fast, so pics will be taken within 15 minutes, as desired
        {
            min = 0;  // reset timeout timer
            trigger(); // take picture
        }
    }
}

int main(void)
{
    init_ports();
    init_osc();
    startup(); // setup sequence
    TMR0_init(); // will use for 15 min timer

    int i;
    while(1)// main loop
    {
        while(!detect);// main loop, program spends majority here, detect active low

        for(i=5; i !=0; i--) // to separate beginning of trigger sequence
        {
            delayUs(50000);
        }
        trigger(); // take picture
        for(i=10; i !=0; i--) // delay after picture
        {
            delayUs(50000);
        }
    }

    return (EXIT_SUCCESS);
}


