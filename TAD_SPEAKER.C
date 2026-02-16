#include "TAD_SPEAKER.H"
#include "TAD_TIMER.H"
#include <xc.h>
#include "pic18f4321.h"

// Configuració del Hardware
#define PIN_SPEAKER LATBbits.LATB3
#define TRIS_SPEAKER TRISBbits.TRISB3

// Estats numerats de la màquina d'estats
#define ST_IDLE         0
#define ST_START_SOUND  1
#define ST_PLAYING      2

// Variables privades (encapsulades)
static unsigned char estat_spk = ST_IDLE;
static unsigned char hTimer;         // El "handle" del timer virtual
static unsigned long durada_tics;    // Temps total de so
static unsigned char tics_toggle;    // Per a la freqüència (més tics = més greu)
static unsigned long proxima_commuta;

void Speaker_Init(void) {
    
    if (TI_NewTimer(&hTimer) == TI_FALS) {
        // Si arriba aquí, és que no tenim prou timers definits al TAD_TIMER.C
    }
    estat_spk = ST_IDLE;
}

void Speaker_Trigger(unsigned char tipus) {
    // Si està lliure o és una alarma (que té prioritat), configurem
    if (estat_spk == ST_IDLE || tipus == SO_ALARMA) {
        switch(tipus) {
            case SO_AGUT:
                // 2 segons = 2000ms. Com que cada tic són 2ms
                durada_tics = 1000; 
                tics_toggle = 1;     // Freqüència alta (commuta cada 2ms)
                break;
            case SO_GREU:
                // 100ms = 50 tics
                durada_tics = 50;   
                tics_toggle = 5;     // Freqüència baixa (commuta cada 10ms)
                break;
            case SO_ALARMA:
                // 10 segons = 5000 tics
                durada_tics = 5000; 
                tics_toggle = 2;     // Freqüència d'alarma
                break;
        }
        estat_spk = ST_START_SOUND;
    }
}

void Speaker_Motor(void) {
    switch (estat_spk) {
        case ST_IDLE:
            PIN_SPEAKER = 0;
            break;

        case ST_START_SOUND:
            TI_ResetTics(hTimer);   // Posem el comptador a 0
            proxima_commuta = 0;
            estat_spk = ST_PLAYING;
            break;

        case ST_PLAYING:
            // 1. Control de l'oscil·lació (freqüència del to)
            if (TI_GetTics(hTimer) >= proxima_commuta) {
                PIN_SPEAKER = !PIN_SPEAKER; // Fem moure la membrana de l'altaveu
                proxima_commuta += tics_toggle;
            }

            // 2. Control del temps total que ha de durar el so
            if (TI_GetTics(hTimer) >= durada_tics) {
                PIN_SPEAKER = 0;
                estat_spk = ST_IDLE; // Hem acabat, tornem a esperar
            }
            break;
    }
}