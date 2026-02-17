
#include "TAD_EXITREQUEST.H"
#include "TAD_TIMER.H"
#include <xc.h>
#include "pic18f4321.h"

#define POLSADOR_PREMUT 0

static unsigned char estat = 0;
static unsigned char ElTimer;

void Pols_Init(){
    TRISBbits.TRISB0 = 1;
    INTCON2bits.RBPU = 0;
    TI_NewTimer(&ElTimer);
}

void Pols_motor(){

    switch (estat){

        case 0:
            if(PORTBbits.RB0 == POLSADOR_PREMUT){
                TI_ResetTics(ElTimer);
                estat = 1;
            }
            break;
        case 1 : 
            if(TI_GetTics(ElTimer) >= 5) {
                estat = 2;
            
            }
            break;
        case 2:
            if(PORTBbits.RB0 != POLSADOR_PREMUT) {
                TI_ResetTics(ElTimer);
                estat = 3;

            }
            break;
        case 3:
            if(TI_GetTics(ElTimer) >= 5) {
                // avisar a controler que s'ha premut EXIT REQUEST 
                estat = 0;
            } 
    }
}