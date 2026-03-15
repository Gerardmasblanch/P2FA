#ifndef TAD_SPEAKER_H
#define TAD_SPEAKER_H

void speaker_init(void);
    // Post: Constructor del TAD. Inicialitza l’altaveu i els timers necessaris.
    // És precondició global haver cridat aquesta funció abans de la resta.

void speaker_stopSound(void);
    // Pre: S’ha cridat speaker_init.
    // Post: Atura qualsevol so que s’estigui reproduint.

void speaker_playAcuteSound(void);
    // Pre: S’ha cridat speaker_init.
    // Post: Reprodueix un so agut curt.

void speaker_playAlarmSound(void);
    // Pre: S’ha cridat speaker_init.
    // Post: Reprodueix el so d’alarma.

void speaker_playPressureSound(void);
    // Pre: S’ha cridat speaker_init.
    // Post: Reprodueix el so associat al temporitzador de PIN.

void speaker_motor(void);
    // Pre: S’ha cridat speaker_init.
    // Post: Gestiona la reproducció dels sons del mòdul.

#endif