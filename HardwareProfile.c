/*
 * File:   HardwareProfile.c
 * Author: alvaj
 * Function: created to ensure the microcontroller's clock and io are set 
 *          correctly
 * Created on April 6, 2016, 2:05 PM
 * 
 */


#include "xc.h"
#include "HardwareProfile.h"

//Function borrowed from Joe Lovelace's embeddedshield.c 
//Initialized configurations for PIC24ep
void Init(void)
{
    RCONbits.SWDTEN=0;                  // Disable Watch Dog Timer

    // Configure Oscillator to operate the device at 40Mhz
    // Fosc= Fin*M/(N1*N2), Fcy=Fosc/2
    // Fosc= 8M*40/(2*2)=80Mhz for 8M input clock
    PLLFBD=40;				// M=40
    CLKDIVbits.PLLPOST=0;		// N1=2
    CLKDIVbits.PLLPRE=0;		// N2=2
    OSCTUN=0;				// Tune FRC oscillator, if FRC is used

    // Clock switching to incorporate PLL
    __builtin_write_OSCCONH(0x01);	// Initiate Clock Switch to Primary
                    // Oscillator with PLL (NOSC=0b011)
    __builtin_write_OSCCONL(0x01);	// Start clock switching
    while (OSCCONbits.COSC != 0b001);	// Wait for Clock switch to occur
    while(OSCCONbits.LOCK!=1) {};       // Wait for PLL to lock
}

void initSwitches(void)
{
     // Switch Specific Pins
    ANSELD = 0x00;                      // turn off analog functionality
    SW1 = 1;                            // default the switches to off
    SW2 = 1;
    _TRISD7 = 0x1;                      // set switch pin to inputs
    _TRISD13 = 0x1;
}

void setRelayIO(void)
{
     // Digital Output Specific Pins
    _TRISF0 = 0x0;			//Set TRISF0 to output
    _TRISF1 = 0x0;          //Set TRISF1 to output
    _TRISF2 = 0x0;			//Set TRISF2 to output
    
    //initially turn off Relays
    RELAY1 = 0;
    RELAY2 = 0;
    LED2 = 0;
}

// initialize the ADC, select Analog input pins
void initADC( int amask)
{
    ANSELA = amask;    // select analog input pins
    AD1CON1 = 0x00E0;   // auto convert after end of sampling
    AD1CSSL = 0;        // no scanning required
    AD1CON3 = 0x1F3F;   // max sample time = 31Tad, Tad = 2 x Tcy
    AD1CON2 = 0;        // use MUXA, AVss and AVdd are used as Vref+/-
    AD1CON1bits.ADON = 1; // turn on the ADC

    // Explorer 16 Development Board Errata (work around 2)
    // RB15 should always be a digital output
    _LATB15 = 0;
    _TRISB15 = 0;
} // InitADC

// sample/convert one analog input
int readADC( int ch)
{
    AD1CHS0  = ch;               // select analog input channel
    AD1CON1bits.SAMP = 1;       // start sampling, automatic conversion will follow
    while (!AD1CON1bits.DONE);   // wait to complete the conversion
    return ADC1BUF0;            // read the conversion result
} // readADC
