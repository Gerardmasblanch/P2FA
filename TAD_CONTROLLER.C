#include "pic18f4321.h"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include "TAD_INTENSITY.h"
#include "TAD_SERIE.H"
#include "TAD_KEYPAD.h"
#include "TAD_HALL.H"
#include "TAD_SPEAKER.H"
#include <xc.h>
#include "pic18f4321.h"

#define LED_OK LATAbits.LATA2
#define LED_ALARMA LATAbits.LATA3

static unsigned char intents = 0;
static unsigned char estat = 0;
static unsigned char pol_hall;
static unsigned char pol_exit_request;
static unsigned char ElTimer; // Timer virtual per controlar els temps d'espera entre estats
static unsigned char Flag_NewdayMsg = 1;
static unsigned char flagPIN = 0; 
static unsigned char flagCAP = 0;
static unsigned char timerNewDay;
static unsigned char flagR = 0; // Flag per controlar l'estat de reset del sistema
static unsigned char pols_hall = 0; // Variable para controlar el estado del sensor Hall

void Init_Controller(){
    // sortida els pins dels leds
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    
    // posem en 1 el led ok i apagem led alarma i reiniciem intents i l'estat
    LED_OK = 1;
    LED_ALARMA = 0;
    intents = 0;
    estat = 0;
    pol_exit_request = 0;
    flagCAP = 0;
    pols_hall = 0;        // Recomanat netejar també aquesta
    Flag_NewdayMsg = 1;

    TI_NewTimer(&ElTimer); 
    TI_NewTimer(&timerNewDay);
    TI_ResetTics(timerNewDay);

}

void Hall_Ences(char ences) {
    if (ences) {
        pols_hall = 1; // Marquem que el pols del hall ha estat activat
    } 
}

void Pols_ExitRequest(char pols) {
    if (pols) {
        pol_exit_request = 1; // boto activat
    }
    
}

void Motor_Controller(){

    switch (estat){

        case 0:

            if (Flag_NewdayMsg == 1) {
                if (!SIO_isBusy() && TI_GetTics(timerNewDay) >= 300) { // Enviem el missatge de "New Day" cada 30 segons si el sistema està inactiu
                    SIO_sendString(MSG_HELLO);
                    Flag_NewdayMsg = 0;
                }
            } else if(pol_exit_request == 1) {   //canvio de moment al boto de exit
                if (!SIO_isBusy()) { 
                    SIO_sendString(MSG_OPEN_EXT);
                    pol_exit_request = 0;
                    TI_ResetTics(ElTimer); // reseteja timer per començar a comptar el temps d'espera per tancar la porta exterior
                    estat = 1;
                }

            } else {
                estat = 0; 
            }
            break;

        case 1: 
            if(TI_GetTics(ElTimer) >= 1000) { 
                TI_ResetTics(ElTimer);
                estat = 2;
                SIO_sendString(MSG_CLOSE_EXT); 
                startIntensity();
                flagPIN = 1;
            } else {
                // Fer sonar el speaker que quan passin els 2 segons deixara de sonar i es mostrarà el missatge
            }
            break;

        case 2: // o intents < 3 o temps de espera < 2 minuts mentres s'esoera PIN del keypad bit a bit fer so per speaker cada 1 segon de 0s a 1:45 min i cada 0,5 segons de 1:45 a 2 minuts 
            
            if (flagPIN && !SIO_isBusy()) {
                SIO_startPin();
                if (flagPIN && !SIO_isBusy()) {
                    SIO_sendString(MSG_ASK_PIN); 
                    flagPIN = 0; 
                }
            }
            
            if(TI_GetTics(ElTimer) >= 60000 || intents == 3) { // 60000 tics = 2 minuts (60000 * 2ms = 120 segundos)
                estat = 8;

                
            } else {

                if(TI_GetTics(ElTimer) >= 52500) { // 52500 tics = 1 minut i 45 segons
                    // Fer sonar el speaker cada 0,5 segons
                } else {
                    // Fer sonar el speaker cada 1 segon
                }

                

                if(SIO_pinReady() && !SIO_isBusy()) {
                        char* pin = SIO_getPIN();
                        if (pin[0] == '1' && pin[1] == '6' && pin[2] == '1' && pin[3] == '2' && pin[4] == 'G' && pin[5] == 'E' && pin[6] == 'R') { // PIN correcte
                            SIO_sendString(MSG_OPEN_INT); // Enviem log de tancament de porta interior
                            TI_ResetTics(ElTimer); // reseteja timer per començar a comptar el temps d'espera per tancar la porta interior
                            estat = 3; // canviem a l'estat de porta interior oberta
                        } else { // PIN incorrecte
                            intents++;
                            SIO_sendString(MSG_DENIED); // Enviem log de "Permission Denied"
                            flagPIN = 1; 
                        }
                }
                
            }
            break;

        case 3: // porta interior oberta, esperant que es tanqui

            if(TI_GetTics(ElTimer) >= 1000) { // 1000 tics = 2 segons
                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
                estat = 4; // tornem a l'estat inicial
                if(!SIO_isBusy()){
                    SIO_sendString(MSG_CLOSE_INT); // Enviem log de tancament de porta interior
                }
            } else {
                // Fer sonar el speaker que quan passin els 2 segons deixara de sonar i es mostrarà el missatge
            }

            break;
        
        case 4: // porta interior tancada, sistema en espera de nova activació
            if(pol_exit_request == 1) {   
                if (!SIO_isBusy()){
                    SIO_sendString(MSG_EXIT_REQ);
                   // SIO_startCapture(); // Iniciem la captura de dades per al log d'Exit Request
                    estat = 5; // canviem a l'estat d'espera de captura
                    flagCAP = 1;
                }
                
            } 
            break;
        case 5: 

            if (flagCAP && !SIO_isBusy()) {
                SIO_sendString(MSG_EXIT_REQ); 
                if (flagCAP && !SIO_isBusy()) {
                    SIO_startCapture();
                    flagCAP = 0; 
                }
            }
            
            if (SIO_captureReady()) {
                    char* captureData = SIO_getCapture();
                    if(captureData[0] == 'Y' && captureData[1] == 'e' && captureData[2] == 's') { // Si la captura indica que es confirma el reset del sistema
                        estat = 6;
                    } else {
                        if(captureData[0] == 'N' && captureData[1] == 'o') { // Si la captura indica que no es confirma el reset del sistema
                            estat = 8;
                        } else {
                           flagCAP = 1; // Si la captura no es ni "Yes" ni "No", tornem a iniciar la captura per esperar una resposta vàlida
                        }
                    }

            }
            break;

        case 6: // S'obren les dues portes i es torna a començar el sistema
            if(!SIO_isBusy()) {
                SIO_sendString(MSG_OPEN_BOTH); // Enviem log de tancament de porta interior
                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
            }
            
            estat = 7; // Tornem a l'estat inicial
            break;

        case 7: // Es tanca la porta exterior i es torna a l'estat inicial
            
            if(!SIO_isBusy()) {
                SIO_sendString(MSG_CLOSE_BOTH); // Enviem log de tancament de porta interior
                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
            }
            
            estat = 0; // Tornem a l'estat inicial
            break;
        case 8: 
            if(!SIO_isBusy()){
                SIO_sendString(MSG_THIEF);
                LED_ALARMA = 1; 
                LED_OK = 0;
                stopIntensity();
                //speaker_playAlarmSound(); // Activem el so de l'altaveu
                
                flagR = 1; // Usem el flag per indicar que hem d'enviar el següent missatge (RESET)
                estat = 9; 
            }
            break;

        case 9:
            // Pas 1: Enviem el missatge de RESET
            if (flagR && !SIO_isBusy()) {
                SIO_sendString(MSG_RESET); 
                flagR = 0; 
                flagCAP = 1; // Usem aquest flag per indicar que ara volem capturar
            }

            // Pas 2: Activem la captura NOMÉS UNA VEGADA
            if (flagCAP && !SIO_isBusy()) {
                SIO_startCapture();
                flagCAP = 0; // El baixem immediatament perquè no torni a entrar aquí
            }

            // Pas 3: Processem la resposta quan estigui llesta
            if (SIO_captureReady()) {
                char* captureData = SIO_getCapture();
                if(captureData[0] == 'Y' && captureData[1] == 'e' && captureData[2] == 's') {
                    speaker_stopSound();
                    Init_Controller();   
                } else {
                    // Si s'equivoca, tornem a demanar el missatge i reiniciem el flux
                    flagR = 1; 
                }
            }
            break;
            
            
    }
}

