#include "pic18f4321.h"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include "TAD_INTENSITY.h"
#include "TAD_SERIAL.H"
#include "TAD_KEYPAD.h"
#include "TAD_HALL.H"
#include "TAD_SPEAKER.H"
#include <xc.h>
#include "pic18f4321.h"

#define LED_OK LATAbits.LATA2
#define LED_ALARMA LATAbits.LATA3

static unsigned char intents = 0;
static unsigned char estat = 0;
static unsigned char pols_hall;
static unsigned char pol_exit_request;
static unsigned char ElTimer; // Timer virtual per controlar els temps d'espera entre estats
static unsigned char Flag_NewdayMsg = 1;
static unsigned char flagPIN = 0; 
static unsigned char flagCAP = 0;
static unsigned char flagR = 0; // Flag per controlar l'estat de reset del sistema
static unsigned char pols_hall = 0; // Variable para controlar el estado del sensor Hall
static unsigned char flagSo = 0;

void Init_Controller(){
    // sortida els pins dels leds
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    
    // posem en 1 el led ok i apagem led alarma i reiniciem intents i l'estat
    LED_OK = 1;
    LED_ALARMA = 0;
    intents = 0;
    estat = 0;
    speaker_stopSound();
    stopIntensity();

    flagCAP = 0;
    Flag_NewdayMsg = 1;
    pols_hall = 0;
    pol_exit_request = 0;
    TI_NewTimer(&ElTimer); 

    

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
                if (!SIO_isBusy()) { // Enviem el missatge de "New Day" cada 30 segons si el sistema està inactiu
                    SIO_sendString(MSG_HELLO);
                    Flag_NewdayMsg = 0;
                }
            } else if(pols_hall == 1) {   //canvio de moment al boto de exit
                if (!SIO_isBusy()) { 
                    SIO_sendString(MSG_OPEN_EXT);
                    speaker_playAcuteSound();
                    pols_hall = 0;
                    TI_ResetTics(ElTimer); // reseteja timer per començar a comptar el temps d'espera per tancar la porta exterior
                    estat = 1;
                }

            } else {
                estat = 0; 
            }
            break;

        case 1: 
            
           
            if (TI_GetTics(ElTimer) >= 1000) {
                speaker_stopSound();
                estat = 2;
                TI_ResetTics(ElTimer);
                SIO_sendString(MSG_CLOSE_EXT); 
                startIntensity();
                speaker_playPressureSound();
                flagPIN = 1;
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

                if(SIO_pinReady() && !SIO_isBusy()) {
                        char* pin = SIO_getPIN();
                        if (pin[0] == '1' && pin[1] == '6' && pin[2] == '1' && pin[3] == '2' && pin[4] == 'G' && pin[5] == 'E' && pin[6] == 'R') { // PIN correcte
                            SIO_sendString(MSG_OPEN_INT); // Enviem log de tancament de porta interior
                            speaker_playAcuteSound();
                            stopIntensity();
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
        
        case 4: // Esperant Exit Request
            if(pol_exit_request == 1) {   
                pol_exit_request = 0;
                flagR = 1;   // Fem servir flagR per enviar el missatge
                flagCAP = 0; // Ens assegurem que la captura està apagada
                estat = 5; 
                
            } 
            break;

        case 5: // Lògica Exit Request (Yes/No)
            // Pas 1: Enviem la pregunta una sola vegada
            if (flagR && !SIO_isBusy()) {
                if(!SIO_isBusy()){
                    SIO_sendString(MSG_EXIT_REQ);
                    flagR = 0; 
                    flagCAP = 1; // Un cop enviat, preparem per capturar
                }
                
            }

            // Pas 2: Iniciem captura només quan la SIO estigui lliure
            if (flagCAP && !SIO_isBusy()) {
                if(!SIO_isBusy()){
                    SIO_startCapture();
                    flagCAP = 0; // Baixem el flag perquè no resetegi la captura en el següent tic
                }
                
            }

            // Pas 3: Processem resposta
            if (SIO_captureReady()) {
                char* captureData = SIO_getCapture();
                if(captureData[0] == 'Y' && captureData[1] == 'e' && captureData[2] == 's') {
                    estat = 6;
                } else if(captureData[0] == 'N' && captureData[1] == 'o') { // Atenció: 'No', no 'NO' (depèn del terminal)
                    estat = 8;
                } else {
                    // Si posa qualsevol altra cosa, tornem a preguntar
                    flagR = 1; 
                }
            }
            break;

        case 6: // S'obren les dues portes i es torna a començar el sistema
            if(!SIO_isBusy()) {
                SIO_sendString(MSG_OPEN_BOTH); // Enviem log de tancament de porta interior
                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
                speaker_playAcuteSound();
                estat = 7; // Tornem a l'estat inicial 
            }
            
            
            break;

        case 7: // Es tanca la porta exterior i es torna a l'estat inicial
            
            if(!SIO_isBusy() && TI_GetTics(ElTimer) >= 1000) {
                SIO_sendString(MSG_CLOSE_BOTH); // Enviem log de tancament de porta interior
                TI_ResetTics(ElTimer); // resetejem el timer per a la pròxima comparació
                Init_Controller();

            }
            break;

        case 8: // Inici Alarma

            if(!SIO_isBusy()){
                SIO_sendString(MSG_THIEF);
                LED_ALARMA = 1; 
                LED_OK = 0;
                stopIntensity();
                speaker_playAlarmSound(); 
                    
                flagR = 1; // Per enviar MSG_RESET
                flagCAP = 0;
                estat = 9; 
            } 
            
            break;

        case 9: // Bucle de Reset
            // Pas 1: Enviem missatge de Reset
            if (flagR && !SIO_isBusy()) {
                SIO_sendString(MSG_RESET); 
                flagR = 0; 
                flagCAP = 1; 
            }

            // Pas 2: Iniciem captura
            if (flagCAP && !SIO_isBusy()) {
                SIO_startCapture();
                flagCAP = 0; 
            }

            // Pas 3: Esperem "Yes"
            if (SIO_captureReady()) {
                char* captureData = SIO_getCapture();
                if(captureData[0] == 'Y' && captureData[1] == 'e' && captureData[2] == 's') {
                    // speaker_stopSound();
                    Init_Controller();   
                } else {
                    // Si no escriu "Yes", tornem a enviar el missatge de Reset
                    flagR = 1; 
                }
            }
            break;
            
            
    }
}