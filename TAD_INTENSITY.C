#include "TAD_TIMER.H"
#include "pic18f4321.h"
#include <xc.h>

// fico 100 nivells de intensitat 
// 60 000 / 100 = 600 tics per nivell
#define TEMPS_NIVELL 4000  // <-- 60 000 / 25 = 2400 tics // 6000
#define NIVELLS 15                                        // 10

unsigned char timerIntesity;
unsigned char timerOSC;
unsigned char estat = 0;
 
unsigned char nivellIntesity = 0;  // indica en que nivell --> BRILLANTOR
unsigned char oscilacioIntesity = 0;  // cuantes oscialacions --> relacio OSC i BRILLANTOR

unsigned char flagIntesity = 0; 
unsigned char flagFinal = 0; // ha arribat al maxim 
unsigned char resetOscilacio = 0; 


void Intensity_init(void) {
    
    ADCON1bits.PCFG = 0x0F; 

    TRISAbits.TRISA4 = 0; 
    LATAbits.LATA4 = 0; 

    estat = 0;

    TI_NewTimer(&timerIntesity);
    TI_NewTimer(&timerOSC);
}

void startIntensity() {
    TI_ResetTics(timerIntesity);
    flagIntesity = 1;
    flagFinal = 0;
    oscilacioIntesity = 0;
    nivellIntesity = 0;
    resetOscilacio = 0;
    estat = 1;
}

void stopIntensity() {
    flagIntesity = 0;
    estat = 0;
    LATAbits.LATA4 = 0; 
}

void Intensity_motor(void) {
    switch (estat) {
        case 0:
            LATAbits.LATA4 = 0;
            if (flagIntesity){
                estat = 1;
                TI_ResetTics(timerIntesity);
                TI_ResetTics(timerOSC);
            }
            break;
        case 1:
            if (TI_GetTics(timerIntesity) >= TEMPS_NIVELL) {  //cada 2400
                TI_ResetTics(timerIntesity);
                
                if (nivellIntesity < NIVELLS) {  // un dels 25
                    nivellIntesity++;    // 2400 * 60 = 60 000 tick = 120 000 ms = 2 m
                    resetOscilacio = 1;
                    TI_ResetTics(timerOSC);
                } else {
                    flagFinal = 1;   
                    estat = 2;          
                    break;
                }
            }

            if(TI_GetTics(timerOSC) >= 1) { 
                oscilacioIntesity++;
                TI_ResetTics(timerOSC);
            }

            if (oscilacioIntesity >= NIVELLS || resetOscilacio) { 
                // Fara la oscilacio depen dels nivells assignats
                // 2400, el primer sera 1000000.. 2400 huecos
                oscilacioIntesity = 0; 
                resetOscilacio = 0;
            }

            if (oscilacioIntesity < nivellIntesity) {
                LATAbits.LATA4 = 1; 
            } else {
                LATAbits.LATA4 = 0; 
            }
            break;
        case 2:
             if (flagFinal) {
                LATAbits.LATA4 = 1;
                estat = 2;
            }
             break;
        default:
            estat = 0;  
             break;
    }
}





