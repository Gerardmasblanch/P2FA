#ifndef HALL_H
#define HALL_H

void HALL_Init(void);
unsigned char HALL_PortaTancada(void); 

// Opcional: Una funció que ens digui si l'estat ha canviat 
// (per detectar el moment exacte del robatori)
unsigned char HALL_HaCanviat(void);

#endif