/* 
 * File:   HardwareProfile.h
 * Author: alvaj
 *
 * Created on April 6, 2016, 1:52 PM
 */

#ifndef HARDWAREPROFILE_H
#define	HARDWAREPROFILE_H

#ifdef	__cplusplus
extern "C" {
#endif
    
// Instruction Clock 40MHz
#define FCY 40000000UL  

// IO pin definition
#define RELAY1 _LATF0//Digital Pin1
#define RELAY2 _LATF1//Digital Pin2
    
// Tactile Switch pin definitions
#define SW1 _RD7
#define SW2 _RD13
    
// Analog  pin definition
#define MSL _RB9           // Moisture sensor pin definition
#define MSL_CH   9         // channel 9 = SEN-13322 moisture sensor
#define MSLMASK  0xFFFF    // make AN9 an analog input

// define LED pins
#define LED2 _LATF2
    
//Initial function definitions
void Init(void);
void setRelayO(void);
void initADC( int amask);   // initialize ADC and analog pins
int readADC( int ch);       // sample/convert one analog input
void Delayus( int t);
void initSwitches(void);
#ifdef	__cplusplus
}
#endif

#endif	/* HARDWAREPROFILE_H */

