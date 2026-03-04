#ifndef TAD_SPEAKER_H
#define	TAD_SPEAKER_H

#include <xc.h>

/**
 * @brief Inicialitza el maquinari del Speaker (RD3) i els timers virtuals.
 * Ha de ser cridada un sol cop a l'inici del programa.
 */
void speaker_init(void);

/**
 * @brief Motor del TAD Speaker.
 * Ha de ser cridat periòdicament dins del bucle principal (while(1)) 
 * per gestionar el so de forma no bloquejant.
 */
void speaker_motor(void);

/**
 * @brief Atura qualsevol so que s'estigui reproduint actualment.
 */
void speaker_stopSound(void);

/**
 * @brief Activa un so agut curt. 
 * Ideal per a confirmacions o obertura de portes.
 */
void speaker_playAcuteSound(void);

/**
 * @brief Activa un so d'alarma continu durant 10 segons.
 */
void speaker_playAlarmSound(void);

/**
 * @brief Activa el mode de so de pressió (2 minuts).
 * Comença amb un bip cada 1s i accelera a un cada 500ms a partir dels 1:45.
 */
void speaker_playPressureSound(void);

#endif	/* TAD_SPEAKER_H */