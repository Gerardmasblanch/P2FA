#include "TAD_HALL.H"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include <xc.h>
#include "pic18f4321.h"

// Hall en RC1
#define HALL_IN          PORTCbits.RC1
#define TRIS_HALL_IN     TRISCbits.TRISC1

// Sin imán = 1, con imán = 0
#define HALL_ACTIU       0

// Tiempo antirrebote (tics del TAD_TIMER)
#define T_REBOTS         5

static unsigned char estat = 1;      // empieza en 1 (sin imán)
static unsigned char timerRebots;

void HALL_Init(void)
{
    TRIS_HALL_IN = 1;               // RC1 entrada

    TI_NewTimer(&timerRebots);
    TI_ResetTics(timerRebots);

    estat = 1;                      // sin imán
}

void Motor_Hall(void)
{
    switch (estat)
    {
        case 1: // estable en 1, esperando que aparezca imán (baje a 0)
            if (HALL_IN == HALL_ACTIU) {
                TI_ResetTics(timerRebots);
                estat = 2;
            }
            break;

        case 2: // antirrebote al detectar 0
            if (TI_GetTics(timerRebots) >= T_REBOTS) {
                if (HALL_IN == HALL_ACTIU) {
                    // detección confirmada (estable en 0)
                    estat = 3;
                } else {
                    // fue rebote, volvemos a esperar
                    estat = 1;
                }
            }
            break;

        case 3: // estamos en 0 (imán presente), esperando que se quite (vuelva a 1)
            if (HALL_IN != HALL_ACTIU) {
                TI_ResetTics(timerRebots);
                estat = 4;
            }
            break;

        case 4: // antirrebote al soltar (volver a 1) y generar evento
            if (TI_GetTics(timerRebots) >= T_REBOTS) {
                if (HALL_IN != HALL_ACTIU) {
                    // soltado confirmado (estable en 1) -> evento único
                    Hall_Ences(1);
                    estat = 1;
                } else {
                    // rebote, sigue en 0 realmente
                    estat = 3;
                }
            }
            break;

        default:
            estat = 1;
            break;
    }
}