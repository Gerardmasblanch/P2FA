

#include "TAD_SPEAKER.h"
#include "TAD_TMR.h"
#include <xc.h>

#define TEMPS_TO_AGUT       1000     // 200ms - 1000 tics
#define TEMPS_ALARMA        5000    // 10s - 5000 tics
#define TEMPS_PIN_TOTAL     60000   // 2m - 60000 tics
#define TEMPS_PIN_FASE1     52500   // 1.45m - 105s - 52500 tics
#define TEMPS_PIN_PERIOD1   500     // 1 s entre bips
#define TEMPS_PIN_PERIOD2   250     // 500 ms entre bips
#define DURADA_BIP          50      // El bip dura 100 ms

#define SEMIPERIODE_TO_AGUT    1       // To agut 
#define SEMIPERIODE_TO_MITJA   3       // To mitjà
#define SEMIPERIODE_TO_GREU       8       // To greu

static unsigned char Estat = 0;

static unsigned char TimerDuracio;   // Durada total
static unsigned char TimerPulse;     // Mode pin
static unsigned char TimerTone;      // Frequència de l'ona quadrada

static unsigned int ToneSemiperiodTics = 0;

static void SPE_StopInternal(void) {
    LATAbits.LATA1 = 0;             
    TI_ResetTics(TimerDuracio);   
    TI_ResetTics(TimerPulse);
    TI_ResetTics(TimerTone);
}

void speaker_init(void) {
    ADCON1bits.PCFG = 0x0F;   // tot dig
    CMCONbits.CM = 0x07;      // sense comparadors
    TRISAbits.TRISA1 = 0;    
    LATAbits.LATA1 = 0;      

    TI_NewTimer(&TimerDuracio);
    TI_NewTimer(&TimerPulse);
    TI_NewTimer(&TimerTone);
    
    Estat = 0;
}

void speaker_stopSound(void) {
    SPE_StopInternal();
    Estat = 0;
}

void speaker_playAcuteSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = SEMIPERIODE_TO_AGUT;
    TI_ResetTics(TimerDuracio);
    TI_ResetTics(TimerTone);
    Estat = 1;
}

void speaker_playAlarmSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = SEMIPERIODE_TO_MITJA;
    TI_ResetTics(TimerDuracio);
    TI_ResetTics(TimerTone);
    Estat = 2;
}

void speaker_playPressureSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = SEMIPERIODE_TO_GREU;
    TI_ResetTics(TimerDuracio);
    TI_ResetTics(TimerPulse);
    TI_ResetTics(TimerTone);
    Estat = 3;
}

void speaker_motor(void) {
    unsigned long tDuracio;
    unsigned long tPulse;
    unsigned long tTone;
    unsigned int periodActualTics;

    switch(Estat) {
        case 0: // Repos
            break;

        case 1: // So agut curt
            tDuracio = TI_GetTics(TimerDuracio);
            if (tDuracio >= TEMPS_TO_AGUT) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }
            tTone = TI_GetTics(TimerTone);
            if (tTone >= ToneSemiperiodTics) {
                LATAbits.LATA1 ^= 1;   //XOR
                TI_ResetTics(TimerTone);
            }
            break;

        case 2: // Alarma (10 s)
            tDuracio = TI_GetTics(TimerDuracio);
            if (tDuracio >= TEMPS_ALARMA) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }
            tTone = TI_GetTics(TimerTone);
            if (tTone >= ToneSemiperiodTics) {
                LATAbits.LATA1 ^= 1;  //XOR
                TI_ResetTics(TimerTone);
            }
            break;

        case 3: // So PIN (2 m)
            tDuracio = TI_GetTics(TimerDuracio);
            if (tDuracio >= TEMPS_PIN_TOTAL) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }

            periodActualTics = (tDuracio < TEMPS_PIN_FASE1) ? TEMPS_PIN_PERIOD1 : TEMPS_PIN_PERIOD2;

            tPulse = TI_GetTics(TimerPulse);
            if (tPulse < DURADA_BIP) {
                tTone = TI_GetTics(TimerTone);
                if (tTone >= ToneSemiperiodTics) {
                    LATAbits.LATA1 ^= 1;  //XOR
                    TI_ResetTics(TimerTone);
                }
            } else {
                LATAbits.LATA1 = 0;
            }

            if (tPulse >= periodActualTics) {
                TI_ResetTics(TimerPulse);
                TI_ResetTics(TimerTone);
            }
            break;

        default:
            SPE_StopInternal();
            Estat = 0;
            break;
    }
}