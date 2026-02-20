#include "HALL.H"
#include "pic18f4321.h"
#include <xc.h>


static unsigned char estat_anterior;

void HALL_Init(void) {
    ADCON1 |= 0x0F;       
    TRISAbits.TRISA0 = 1; // entrada
    estat_anterior = PORTAbits.RA0;
}

//NO CAL SEGURAMENT
unsigned char HALL_PortaTancada(void) {
    return PORTAbits.RA0; 
}

// funcio que es cridara des del controller per saber si ha canviat el valor del hall
// per obrir la primera porta
unsigned char HALL_HaCanviat(void) {
    unsigned char estat_actual = PORTAbits.RA0;
    if (estat_actual != estat_anterior) {
        estat_anterior = estat_actual;
        return 1;
    }
    return 0;
}