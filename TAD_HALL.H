#ifndef TAD_HALL_H
#define TAD_HALL_H

void HALL_Init(void);
    // Pre: Haver cridat aquesta funció abans de la resta.
    // Post: Constructor del TAD. Inicialitza el sensor Hall i el timer d’anti-rebots.

void Motor_Hall(void);
    // Pre: S’ha cridat HALL_Init.
    // Post: Detecta una activació estable del sensor Hall i genera l’esdeveniment.

#endif