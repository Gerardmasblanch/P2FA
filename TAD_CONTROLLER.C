#include "pic18f4321.h"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include "TAD_INTESITY.H"
#include "TAD_SERIE.H"
#include "TAD_HALL.H"
#include "TAD_SPEAKER.H"
#include <xc.h>
#include "pic18f4321.h"

#define LED_OK LATDbits.LATD0
#define LED_ALARMA LATDbits.LATD1

unsigned char intents = 0;
unsigned char estat = 0;
unsigned char pol_hall;
unsigned char pol_exit_request;
unsigned char ElTimer; // Timer virtual per controlar els temps d'espera entre estats
unsigned char Flag_Newdaymsg = 0; // Flag per controlar l'enviament del missatge de "New Day" només un cop al dia


void Init_Controller(){

    // sortida els pins dels leds
    TRISDbits.RD0 = 0;
    TRISDbits.RD1 = 0;
    
    // posem en 1 el led ok i apagem led alarma i reiniciem intents i l'estat
    LED_OK = 1;
    LED_ALARMA = 0;
    intents = 0;
    estat = 0;
    TI_NewTimer(ElTimer); // Creem un timer virtual per controlar els temps d'espera entre estats

}

void Hall_Ences(char ences) {
    if (ences) {
        pols_hall = 1; // Marquem que el pols del hall ha estat activat
    } 
}

void Pols_ExitRequest(char pols) {
    if (pols) {
        pol_exit_request = 1; // Marquem que el pols de l'Exit Request ha estat activat
    }
    
}

void Motor_Controller(){

    switch (estat){

        case 0: // estat inicial, esperant que s'obri la porta exterior
            if (Flag_Newdaymsg == 0) {
                SERIAL_EnviarLog(MSG_NEW_DAY); // Enviem log de "New Day"
                Flag_Newdaymsg = 1; // Marquem que ja hem enviat el missatge
            }

            if(pols_hall == 1) {
                SERIAL_EnviarLog(MSG_OPEN_EXT); // Enviem log d'obertura de porta exterior
                pols_hall = 0; // Resetejem el pols del hall per evitar múltiples activacions
                estat = 1;
            }
            break;

        case 1: // porta exterior oberta, esperant que es tanqui 2 segons per tancar la porta i fer so per speaker.
            if(TI_GetTics(ElTimer) >= 1000) { // 1000 tics = 2 segons 10 Mhz per tant cada tic son 2 ms, per tant 1000 tics son 2s
                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
                estat = 2;
                SERIAL_EnviarLog(MSG_CLOSE_EXT); // Enviem log de tancament de porta exterior
            }
            break;

        case 2: // o intents < 3 o temps de espera < 2 minuts mentres s'esoera PIN del keypad bit a bit fer so per speaker cada 1 segon de 0s a 1:45 min i cada 0,5 segons de 1:45 a 2 minuts 
            if(TI_GetTics(ElTimer) < 60000 || intents < 3) { // 60000 tics = 2 minuts (60000 * 2ms = 120 segundos)
                //fer so cada 1 segon fins el 1:45 min i cada 0,5 segons fins els 2 minuts

                if(TI_GetTics(ElTimer) < 52500) { // 52500 tics = 1:45 minuts (52500 * 2ms = 105 segundos)
                    //SPEAKER_FerSo(1); // Fer so cada 1 segon
                } else {
                    //SPEAKER_FerSo(0.5); // Fer so cada 0,5 segons
                }


                // Aquí hauries de posar el codi per rebre el PIN del keypad bit a bit i comparar-lo i mostrarlo per pantalla 
                // Si el PIN és correcte:

                if (/* PIN correcte */) {
                    SERIAL_EnviarLog(MSG_OPEN_INT); // Enviem log d'obertura de porta interior
                    LED_OK = 1; // Encenem el led d'ok
                    estat = 3; // passem a l'estat de porta interior oberta
                } else {
                    SERIAL_EnviarLog(MSG_DENIED); // Enviem log de "Permission Denied"
                    LED_OK = 0; // Apaguem el led d'ok
                    intents++; // Incrementem els intents
                }

                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
                intents++; // incrementem els intents

            } else {
                SERIAL_EnviarLog(MSG_THIEF); // Enviem log de "THIEF INTERCEPTED!"
                LED_ALARMA = 1; // Encenem el led d'alarma
                
            }
            break;

        case 3: // porta interior oberta, esperant que es tanqui
            if(HALL_HaCanviat()) {
                intents = 0; // resetejem intents per si de cas algu ha entrat i vol tornar a sortir
                estat = 0; // tornem a l'estat inicial
            }
            break;
    }
}