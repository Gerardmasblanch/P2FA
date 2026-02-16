#ifndef SPEAKER_H
#define SPEAKER_H

#include <xc.h>

// Tipus de sons segons l'enunciat
#define SO_AGUT   1   // Portes: 2 segons
#define SO_GREU   2   // Beep PIN: 100 ms
#define SO_ALARMA 3   // Alarma: 10 segons

// Funcions públiques
void Speaker_Init(void);
void Speaker_Motor(void);
void Speaker_Trigger(unsigned char tipus);

#endif