#ifndef TAD_KEYPAD_H
#define TAD_KEYPAD_H

void KEY_Init(void);
    // Pre: Haver cridat aquesta funció abans de la resta.
    // Post: Constructor del TAD. Inicialitza el keypad i els timers necessaris.

void KEY_Motor(void);
    // Pre: S’ha cridat KEY_Init.
    // Post: Escaneja el keypad, tracta els rebots i gestiona la introducció de tecles.

#endif