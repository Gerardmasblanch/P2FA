#ifndef TAD_INTENSITY_H
#define TAD_INTENSITY_H

void Intesity_init(void);
    // Pre: Haver cridat aquesta funció abans de la resta.
    // Post: Constructor del TAD. Inicialitza el control d’intensitat i els timers necessaris.

void startIntensity(void);
    // Pre: S’ha cridat Intesity_init.
    // Post: Inicia el procés d’increment progressiu de la intensitat.

void stopIntensity(void);
    // Pre: S’ha cridat Intesity_init.
    // Post: Atura el control d’intensitat i apaga la sortida.

void Intesity_motor(void);
    // Pre: S’ha cridat Intesity_init.
    // Post: Gestiona el funcionament del mòdul d’intensitat.

#endif