#ifndef TAD_POLS_H
#define TAD_POLS_H

void Pols_Init(void);
    // Pre: Haver cridat aquesta funció abans de la resta.
    // Post: Constructor del TAD. Inicialitza el polsador i el timer d’anti-rebots.
     
void Pols_motor(void);
    // Pre: S’ha cridat Pols_Init.
    // Post: Detecta una pulsació estable i genera la petició de sortida.

#endif