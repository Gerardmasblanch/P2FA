#include "TAD_HALL.H"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include <xc.h>
#include "pic18f4321.h"

#define T_REBOTS         5 // en tics, 5 tics = 10 ms 

static unsigned char estat;      
static unsigned char timerRebots;

void HALL_Init(void)
{
    TRISCbits.TRISC1 = 1;              
    TI_NewTimer(&timerRebots);
    TI_ResetTics(timerRebots);
    estat = 0;                    
}

void Motor_Hall(void)
{
    switch (estat){
        case 0: 
            if (PORTCbits.RC1 == 0) {
                TI_ResetTics(timerRebots);
                estat = 1;
            }
            break;

        case 1: 
            if (TI_GetTics(timerRebots) >= T_REBOTS) {
                if (PORTCbits.RC1 == 0) {
                    estat = 2;
                } else {
                    estat = 1;
                }
            }
            break;

        case 2: 
            if (PORTCbits.RC1 != 0) {
                TI_ResetTics(timerRebots);
                estat = 3;
            }
            break;

        case 3: 
            if (TI_GetTics(timerRebots) >= T_REBOTS) {
                if (PORTCbits.RC1 != 0) {
                    Hall_Ences(1);
                    estat = 0;
                } else {
                    estat = 2;
                }
            }
            break;

        default:
            estat = 0;
            break;
    }
}