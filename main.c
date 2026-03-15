#include "TAD_SERIAL.H"
#include "TAD_HALL.H"
#include "TAD_INTENSITY.h"
#include "TAD_EXITREQUEST.H"
#include "TAD_CONTROLLER.H"
#include "TAD_KEYPAD.H"
#include "TAD_SPEAKER.H"
#include "TAD_TIMER.H"
#include <xc.h>

#pragma config OSC = HS
#pragma config PBADEN = DIG
#pragma config MCLRE = ON
#pragma config DEBUG = OFF
#pragma config PWRT = OFF
#pragma config BOR = OFF
#pragma config WDT = OFF
#pragma config LVP = OFF

void __interrupt() isr(void)
{
    if (INTCONbits.TMR0IF && INTCONbits.TMR0IE) {
        RSI_Timer0();
    }
}

void main(void) {   
    
    TI_Init(); 
    SIO_Init();
    Pols_Init();
    Init_Controller();
    Intesity_init();
    KEY_Init();
    HALL_Init();
    speaker_init();

    ei();
    while(1){
        
        Motor_Controller();
        SIO_Motor();
        Pols_motor();
        Intesity_motor();
        KEY_Motor();
        Motor_Hall();
        speaker_motor();
    }
}