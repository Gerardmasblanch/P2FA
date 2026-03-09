/*
 * File:   TAD_SPEAKER.c
 * Author: Ari i Marc (Adaptat per a RA1 i TAD_TIMER)
 *
 * Created on 21 de febrer de 2026
 */

#include "TAD_SPEAKER.h"
#include "TAD_TMR.h"
#include <xc.h>

/* Constants en tics sabent que el tick és de 2 ms */
#define ACUTE_DURATION      1000     /* so agut curt (200ms) */
#define ALARM_DURATION      5000    /* alarma 10 s */
#define PRESSURE_TOTAL      60000   /* 2 minuts */
#define PRESSURE_PHASE1     52500   /* 1:45 = 105 s */

#define PRESSURE_PERIOD1    500     /* 1 s entre bips */
#define PRESSURE_PERIOD2    250     /* 500 ms entre bips */
#define SOUND_PULSE         50      /* El bip dura 100 ms */

#define ACUTE_SEMIPERIOD    1       /* To agut (2ms per semiperíode) */
#define ALARM_SEMIPERIOD    3       /* To mitjà */
#define GRAVE_SEMIPERIOD    8       /* To greu */

/*=====================================================
  VARIABLES PRIVADES
=====================================================*/
static unsigned char Estat = 0;

/* Identificadors dels timers virtuals gestionats pel TAD_TIMER */
static unsigned char TimerDuracio;   /* Durada total del mode actual */
static unsigned char TimerPulse;     /* Cadència de silenci/so (mode pressió) */
static unsigned char TimerTone;      /* Freqüència de l'ona quadrada */

static unsigned int ToneSemiperiodTics = 0;

/*=====================================================
  FUNCIONS PRIVADES
=====================================================*/

static void SPE_StopInternal(void) {
    LATAbits.LATA1 = 0;             /* Assegurem pin a 0 (silenci) */
    TI_ResetTics(TimerDuracio);     /* Reiniciem comptadors del TAD_TIMER */
    TI_ResetTics(TimerPulse);
    TI_ResetTics(TimerTone);
}

/*=====================================================
  MOTOR DEL TAD (Scheduler Cooperatiu)
=====================================================*/
void speaker_motor(void) {
    unsigned long tDuracio;
    unsigned long tPulse;
    unsigned long tTone;
    unsigned int periodActualTics;

    switch(Estat) {
        case 0: /* IDLE - Repòs */
            break;

        case 1: /* SO AGUT CURT */
            tDuracio = TI_GetTics(TimerDuracio);
            if (tDuracio >= ACUTE_DURATION) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }
            tTone = TI_GetTics(TimerTone);
            if (tTone >= ToneSemiperiodTics) {
                LATAbits.LATA1 ^= 1;  /* Commutem RA1 */
                TI_ResetTics(TimerTone);
            }
            break;

        case 2: /* ALARMA CONTINUA (10 s) */
            tDuracio = TI_GetTics(TimerDuracio);
            if (tDuracio >= ALARM_DURATION) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }
            tTone = TI_GetTics(TimerTone);
            if (tTone >= ToneSemiperiodTics) {
                LATAbits.LATA1 ^= 1;
                TI_ResetTics(TimerTone);
            }
            break;

        case 3: /* MODE PRESSIÓ (2 minuts) */
            tDuracio = TI_GetTics(TimerDuracio);
            if (tDuracio >= PRESSURE_TOTAL) {
                SPE_StopInternal();
                Estat = 0;
                break;
            }

            /* Determinem el ritme segons la fase (1s o 0.5s) */
            periodActualTics = (tDuracio < PRESSURE_PHASE1) ? PRESSURE_PERIOD1 : PRESSURE_PERIOD2;

            tPulse = TI_GetTics(TimerPulse);
            if (tPulse < SOUND_PULSE) {
                /* Dins del bip: generem ona quadrada */
                tTone = TI_GetTics(TimerTone);
                if (tTone >= ToneSemiperiodTics) {
                    LATAbits.LATA1 ^= 1;
                    TI_ResetTics(TimerTone);
                }
            } else {
                /* Fora del bip: silenci */
                LATAbits.LATA1 = 0;
            }

            /* Reiniciem el període de la cadència */
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

/*=====================================================
  FUNCIONS PÚBLIQUES
=====================================================*/

void speaker_init(void) {
    /* Configurem RA1 com a digital i sortida */
    ADCON1 |= 0x0F;
    CMCON |= 0x07;/* Desactiva entrades analògiques al Port A */
    TRISAbits.TRISA1 = 0;    /* RA1 com a sortida */
    LATAbits.LATA1 = 0;      /* Inicialment a 0 */

    /* Demanem 3 timers al TAD_TIMER */
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
    ToneSemiperiodTics = ACUTE_SEMIPERIOD;
    TI_ResetTics(TimerDuracio);
    TI_ResetTics(TimerTone);
    Estat = 1;
}

void speaker_playAlarmSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = ALARM_SEMIPERIOD;
    TI_ResetTics(TimerDuracio);
    TI_ResetTics(TimerTone);
    Estat = 2;
}

void speaker_playPressureSound(void) {
    speaker_stopSound();
    ToneSemiperiodTics = GRAVE_SEMIPERIOD;
    TI_ResetTics(TimerDuracio);
    TI_ResetTics(TimerPulse);
    TI_ResetTics(TimerTone);
    Estat = 3;
}