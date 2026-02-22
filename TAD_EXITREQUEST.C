
#include "TAD_EXITREQUEST.H"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include <xc.h>
#include "pic18f4321.h"

#define POLSADOR_PREMUT 0

static unsigned char estat = 0;
static unsigned char timerRebots;

void Pols_Init(){
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    
    TRISBbits.TRISB0 = 1;
    INTCON2bits.RBPU = 0;
    TI_NewTimer(&timerRebots);
}

void Pols_motor(){

    switch (estat){

        case 0:
            if(PORTBbits.RB0 == POLSADOR_PREMUT){
                TI_ResetTics(timerRebots);
                estat = 1;
            } else {
                estat = 0;
            }
            break;
        case 1 : 
            if(TI_GetTics(timerRebots) >= 5) {
                estat = 2;
            } else {
                estat = 1;
            }
            break;
        case 2:
            if(PORTBbits.RB0 != POLSADOR_PREMUT) {
                TI_ResetTics(timerRebots);
                LATAbits.LATA4 = 1;
                estat = 3;
            } else {
                estat = 2;
            }
            break;
        case 3:
            if(TI_GetTics(timerRebots) >= 5) {
                Pols_ExitRequest(1); 
                estat = 0;
            } else {
                estat = 3;
            }
            break;
        default:
            estat = 0;
            break;
    }
}