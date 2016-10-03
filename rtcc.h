#ifndef RTCC_H
#define	RTCC_H

#ifdef	__cplusplus
extern "C" {
#endif
#define RTCCUnlock()    RCFGCALbits.RTCWREN = 1;
#define RTCCLock()      RCFGCALbits.RTCWREN = 0;
#define RTCCOn()        RCFGCALbits.RTCEN = 1;
#define RTCCOff()       RCFGCALbits.RTCEN = 0;
#define EnableSecOsc()  __builtin_write_OSCCONL( 0x02 );

extern void __attribute__ ( (interrupt, no_auto_psv) )  _RTCCInterrupt( void );

void InitRTCC( void)
{   
    // unlock and enable SOSC
    __builtin_write_OSCCONL( 2);    
 
    // unlock RCFGCAL, set RTCWREN
    __builtin_write_RTCWEN();    

    _RTCEN = 0;         // disable the clock

    // set 12/01/2006 WED 12:01:30
    _RTCPTR = 1;        // start the sequence
    RTCVAL = 0x0312;    // WEEKDAY/HOURS
    RTCVAL = 0x0130;    // MINUTES/SECONDS
    
    // optional calibration
    //_CAL = 0x00;      

    // lock and enable 
    _RTCEN = 1;         // start the clock
    _RTCWREN = 0;       // lock 

} // InitRTCC
#ifdef	__cplusplus
}
#endif

#endif	/* RTCC_H */