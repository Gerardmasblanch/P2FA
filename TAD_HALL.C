#include "TAD_HALL.H"
#include "TAD_CONTROLLER.H"
#include "pic18f4321.h"
#include <xc.h>


static unsigned char estat_anterior;
static unsigned char pols_hall; // Variable para marcar que el sensor de efecto Hall ha sido activado, para evitar múltiples activaciones por ruido o rebotes del sensor. Se resetea en el controlador después de procesar la activación.

void HALL_Init(void) {
    ADCON1 |= 0x0F;       
    TRISBbits.TRISB7 = 1; // entrada
    estat_anterior = PORTBbits.RB7;
}




// funcio que es cridara des del controller per saber si ha canviat el valor del hall
// per obrir la primera porta
void Motor_Hall(void) {

    switch (estat_anterior) {
        
        case 0: // Si el valor anterior era 0, esperem que passi a 1
            if (PORTBbits.RB7 == 1) {
                estat_anterior = 1; // Actualitzem el valor anterior
            }
            break;
        case 1: // Si el valor anterior era 1, esperem que passi a 0
            if (PORTBbits.RB7 == 0) {
                estat_anterior = 2; // Actualitzem el valor anterior
            }
            break;
        case 2: 

            Hall_Ences(1);
            estat_anterior = 0; // Resetejem el valor anterior per a la pròxima detecció
            break;
    }
}