/*~~~~~~~~~~~~~~~~~~~EECE-6038: Advanced Microsystems~~~~~~~~~~~~~~~~~~~*       
 * Author:          Alva Webster
 * Professor:       Fred Beyette
 * Date Created:    March 18th, 2016    
 * 
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*     
 * Files:
 * 
 * main.c = the main program           
 * HardwareProfile.c = initiates the ports for the pins that need to be
 *                     used in program
 * rtcc.h = activates RTCC clock and pins needed for the real time clock
 *          module
 * HardwareProfile = links the main program with the HardwareProfile.c file
 *                   file 
 * 
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * PROGRAM DESCRIPTION:
 * 
 * The program serves as the greenhouse automation project Plant Plant.
 * The program activates relays to power a pump and lights. The plant
 * also monitors soil moisture level to activate the pump to water the 
 * plant. 
 * 
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*
 * ADDITIONAL NOTES:
 * Relay 1 serves as the relay for the lights.
 * Relay 2 serves as the relay for the pump.
 * Good soil has an input of between 400 and 600. 
 * 
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

#include "xc.h"
#include "HardwareProfile.h"
#include "rtcc.h"

_FOSCSEL(FNOSC_PRIPLL)          // Primary clock is 8MHz Crystal with PLL
_FOSC(FCKSM_CSECMD & OSCIOFNC_OFF  & POSCMD_XT)
      // Clock Switching is enabled and Fail Safe Clock Monitor is disabled
      // OSC2 Pin Function: OSC2 is Clock Output
      // Primary Oscillator Mode: XT Crystal
_FWDT(FWDTEN_OFF) 		 // Watchdog Timer Enabled/disabled by user software
_FICD(JTAGEN_OFF & ICS_PGD1)    // Disable JTAG pins & Setup program pins
        
//declare global variables for use functions
int secondCounter = 0;
int minuteCounter = 0;
int runLightsRTCC = 0;
int runLightsOVR = 0;

int runPumpOVR = 0;
int runPump = 0;
int runPumpSEQ = 0;
int pumpSecondCounter = 0;

double moisture;

#define LED_BLED                        _RD2     //defines pin definitonns of LED backlack
#define ConfigureBackLight()            _TRISD2 = 0 //set initialization of backlight to 0
//define DELAY1 = 100;

//RTCC interrupt service routine, triggered every second
void _ISR _RTCCInterrupt( void)
{
    LED_BLED = 1 - LED_BLED; // toggle Display Backlight
    secondCounter++;//increment second counter
    pumpSequence();//initialize pump sequence
    
    //check if interrupt sequence has counted a minute
    if(secondCounter == 60)
    {
        //increment minute counter by 1 for every 60 seconds
        minuteCounter++;
        
        //has it been five minutes?
        if(minuteCounter == 5)
        {
           minuteCounter = 0;
           secondCounter = 0;
        }
        
        //set seconds to 0
        else
        {
            secondCounter = 0;
        }
    }
    //checks if lights should be on
    else
    {  
        //the minute counter is between certain times of the day
        if(minuteCounter >= 1 && minuteCounter <= 4)
        {
            //run lights variable
            runLightsRTCC = 1;
        }
        else
        {
            //off lights variable
            runLightsRTCC = 0;
        }
    }
    _RTCIF = 0;     // clear flag
}

//pump function
void pumpSequence(void)
{
    //if it is saying to run the pump and moisure sensor value is low
    if(runPumpSEQ == 1 && moisture <= 200)
    {
        //increment pump second counter
        pumpSecondCounter++;
        //turn pump on for 30 seconds
        if(pumpSecondCounter <= 30)
        {
            runPump = 1;//turn pump on
        }
        else
        {
            //set pump counter to 0
            pumpSecondCounter = 0;
            runPump = 0;
        }
    }
    
    //don't run pump is moisture sensor is low
    else
    {
        runPump = 0;
        pumpSecondCounter = 0;
    }
}

//grabs adc value from moisture sensor output
double callADC(double input)
{
    double ADCValue;       //define variable for store
    initADC(MSLMASK);      // Temp mask defines the mask to select AN9 from
    ADCValue = readADC(9); //read the ADC from function
    input = ADCValue;     // moisture senosr input
    return input;
}

//activates relays for hardware use
void onRelays(void)
{
    //RELAY for Lights Conditions
    if(runLightsOVR == 1 && runLightsRTCC == 1)
    {
        RELAY1 = 0;
        Delayus(200);
    }
    else if(runLightsOVR == 1 && runLightsRTCC == 0)
    {
        RELAY1 = 0;
        Delayus(200);
    }
    else if(runLightsOVR == 0 && runLightsRTCC == 1)
    {
        RELAY1 = 0;
        Delayus(200);
    }
    else
    {
        RELAY1 = 1;
        Delayus(200);
    }
    
    //RELAY for pump conditions
    if(runPumpOVR == 1 && runPump == 1)
    {
        RELAY2 = 0;
        Delayus(200);
    }
    else if(runPumpOVR == 1 && runPump == 0)
    {
        RELAY2 = 0;
        Delayus(200);
    }
    else if(runPumpOVR == 0 && runPump == 1)
    {
        RELAY2 = 0;
        Delayus(200);
    }
    else
    {
        RELAY2 = 1;
        Delayus(200);
    }
}

//check if the override buttons on the development board are pressed
void checkButtons(void)
{
    if (SW1 == 0 && SW2 == 0)
    {       
        runLightsOVR = 1;           
        runPumpOVR = 1;           
    }
    else if(SW1 == 0 && SW2 == 1)
    {
        runLightsOVR = 1;       
        runPumpOVR = 0;           
    }
    else if(SW1 == 1 && SW2 == 0)
    {
        runLightsOVR = 0;       
        runPumpOVR = 1; 
    }
    else
    {
        runLightsOVR = 0;
        runPumpOVR = 0;
    }
    
    if(SW2 == 0)
    {          // check switch one
        runPumpOVR = 1;           // if pressed (active low) turn on LED 0 (LSB)
        Delayus(200);
    }   
    else
    {
        runPumpOVR = 0;
    }                    
}

//compare moisture value to threshold
void checkMoisture(double moisture)
{
    if(moisture <= 200)
    {
        runPumpSEQ = 1;
        Delayus(200);
    }
    else
    {
        runPumpSEQ = 0;
    }
}

//a delay function for delaying based off a millisecond input into the function
void Delayus(int t)
{
    T1CON = 0x8000;     // enable tmr1, Tcy, 1:1
    while (t>0)         // wait for t (msec)
    {
        TMR1 = 0;
        while ( TMR1 < (FCY/3800)); // wait 1us
        t=t-1;
    }
} // Delayms

//sets alarm for the interrupt sequence
void SetALARM( void)
{
    // disable alarm
    _ALRMEN = 0;

    // set the repeat counter
    _ARPT = 0;          // once
    _CHIME = 1;         // no reload

    // set the alarm mask 
    _AMASK = 0b0001;    // every minute
    _ALRMEN = 1;        // enable alarm
    // Set interrupt flags
    _RTCIF = 0;         // clear flag
    _RTCIE = 1;         // enable interrupt

} // Set ALARM

//main function
int main(void) 
{
    
    //Initialize 
    _TRISD2 = 0;
    Init();
    setRelayIO();
    initSwitches();
    
    //Enable RTCC functions
    InitRTCC();
    EnableSecOsc();
    SetALARM();
    
    //a continuous run of functions. Program stops when power to board is lost
    while(1)
    {
        //call functions for use for variables
        onRelays();
        checkButtons();
        moisture = callADC(moisture);
        checkMoisture(moisture);
        
        //added for debugging purposes
        Nop();
        Nop();
        Nop();
    };
    return 0;
}
