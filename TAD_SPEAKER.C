#include "TAD_SPEAKER.h"
#include "TAD_TIMER.h"
#include <xc.h>

/* Constants en tics (1 tic = 2 ms) */
#define ACUTE_DURATION      1000    /* so agut curt (aprox 200ms) */
#define ALARM_DURATION      5000    /* alarma 10 s */
#define PRESSURE_TOTAL      60000   /* 2 minuts */
#define PRESSURE_PHASE1     52500   /* 1:45 = 105 s */

#define PRESSURE_PERIOD1    500     /* 1 s entre bips */
#define PRESSURE_PERIOD2    250     /* 500 ms entre bips */
#define SOUND_PULSE         50      /* El bip dura 100 ms */

#define ACUTE_SEMIPERIOD    1       /* To més ràpid */
#define ALARM_SEMIPERIOD    2       
#define GRAVE_SEMIPERIOD    4       /* To més lent (greu) */

/*=====================================================
  VARIABLES PRIVADES
=====================================================*/
static unsigned char Estat = 0;

static unsigned char *TimerDuracio;   /* Durada total de l'estat */
static unsigned char *TimerPulse;     /* Temps per la cadència (on/off del bip) */
static unsigned char *TimerTone;      /* Temps per la freqüència (ona quadrada) */

static unsigned int ToneSemiperiodTics = 0;

/*=====================================================
  FUNCIONS PRIVADES
=====================================================*/

static void SPE_StopInternal(void) {
    LATAbits.LATA1 = 0;
    timer_resetTics(&TimerDuracio);
    timer_resetTics(&TimerPulse);
    timer_resetTics(&TimerTone);
}

/*=====================================================
  MOTOR DEL TAD
=====================================================*/
void speaker_motor(void) {
    unsigned int tDuracio;
    unsigned int tPulse;
    unsigned int tTone;
    unsigned int periodActualTics;

    switch(Estat) {
        case 0: /* IDLE */
            break;

        case 1: /* SO AGUT CURT */
            tDuracio = timer_getTics(TimerDuracio);
            if (tDuracio >= ACUTE_DURATION) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }
            tTone = timer_getTics(TimerTone);
            if (tTone >= ToneSemiperiodTics) {
                LATAbits.LATA1 ^= 1; 
                timer_resetTics(TimerTone);
            }
            break;

        case 2: /* ALARMA CONTINUA 10S */
            tDuracio = timer_getTics(TimerDuracio);
            if (tDuracio >= ALARM_DURATION) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }
            tTone = timer_getTics(TimerTone);
            if (tTone >= ToneSemiperiodTics) {
                LATAbits.LATA1 ^= 1;
                timer_resetTics(TimerTone);
            }
            break;

        case 3: /* MODE PRESSIÓ (2 MINUTS) */
            tDuracio = timer_getTics(TimerDuracio);
            if (tDuracio >= PRESSURE_TOTAL) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }

            // Decidim si estem en fase lenta (1s) o ràpida (0.5s)
            periodActualTics = (tDuracio < PRESSURE_PHASE1) ? PRESSURE_PERIOD1 : PRESSURE_PERIOD2;

            tPulse = timer_getTics(TimerPulse);
            if (tPulse < SOUND_PULSE) {
                // Estem dins del "BIP" actiu
                tTone = timer_getTics(TimerTone);
                if (tTone >= ToneSemiperiodTics) {
                    LATAbits.LATA1 ^= 1;
                    timer_resetTics(TimerTone);
                }
            } else {
                // Estem en el silenci entre bips
                LATAbits.LATA1 = 0;
            }

            // Reiniciem el cicle del bip (el període)
            if (tPulse >= periodActualTics) {
                timer_resetTics(TimerPulse);
                timer_resetTics(TimerTone);
            }
            break;

        default:
            SPE_StopInternal();
            Estat = 0;
            break;
    }
}

/*=====================================================
  FUNCIONS PÚBLIQUES
=====================================================*/

void speaker_init(void) {
    TRISAbits.RA1 = 0; // Configura RD3 com sortida
    LATAbits.LATA1 = 0;

    timer_newTimer(&TimerDuracio);
    timer_newTimer(&TimerPulse);
    timer_newTimer(&TimerTone);

    Estat = 0;
}

void speaker_stopSound(void) {
    SPE_StopInternal();
    Estat = 0;
}

void speaker_playAcuteSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = ACUTE_SEMIPERIOD;
    Estat = 1;
}

void speaker_playAlarmSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = ALARM_SEMIPERIOD;
    Estat = 2;
}

void speaker_playPressureSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = GRAVE_SEMIPERIOD;
    Estat = 3;
}