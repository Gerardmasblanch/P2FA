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
static unsigned char ElTimer; 
static unsigned char Flag_NewdayMsg = 1;
static unsigned char flagPIN = 0; 
static unsigned char flagCAP = 0;
static unsigned char flagR = 0; 
static unsigned char pols_hall = 0;
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

        case 1:  // esperem 2 segons i enviem missatge per tancar porta exterior.
            
           
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

        case 2: // espera PIN correcte i gestiona intents i els 2 minuts per introduir el PIN
            
            if (flagPIN && !SIO_isBusy()) {
                SIO_startPin();
                if (flagPIN && !SIO_isBusy()) {
                    SIO_sendString(MSG_ASK_PIN); 
                    flagPIN = 0; 
                }
            }
            
            if(TI_GetTics(ElTimer) >= 60000 || intents == 3) { // 60000 tics = 2 minuts
                estat = 8;
            } else {

                if(SIO_pinReady() && !SIO_isBusy()) {
                        char* pin = SIO_getPIN();
                        if (pin[0] == '1' && pin[1] == '6' && pin[2] == '1' && pin[3] == '2' && pin[4] == 'G' && pin[5] == 'E' && pin[6] == 'R') { // PIN correcte
                            SIO_sendString(MSG_OPEN_INT); 
                            speaker_playAcuteSound();
                            stopIntensity();
                            TI_ResetTics(ElTimer); 
                            estat = 3; 
                        } else { // PIN incorrecte
                            intents++;
                            SIO_sendString(MSG_DENIED); // Enviem log de "Permission Denied"
                            flagPIN = 1; 
                        }
                }
                
            }
            break;

        case 3: // obre porta interior i espera 2 segons a tancar-la.

            if(TI_GetTics(ElTimer) >= 1000) { // 1000 tics = 2 segons
                TI_ResetTics(ElTimer); 
                estat = 4; 
                if(!SIO_isBusy()){
                    SIO_sendString(MSG_CLOSE_INT); 
                }
            }

            break;
        
        case 4: // Esperant Exit Request
            if(pol_exit_request == 1) {   
                pol_exit_request = 0;
                flagR = 1;   // flagR per mostrar el missatge.
                flagCAP = 0; // flagCAP per a controlar la captura de missatges.
                estat = 5; 
                
            } 
            break;

        case 5: // Lògica de resposta a l'Exit request on enviem missatges i comprovem si la resposta és si o no.
            
            if (flagR && !SIO_isBusy()) {
                if(!SIO_isBusy()){
                    SIO_sendString(MSG_EXIT_REQ);
                    flagR = 0; 
                    flagCAP = 1; // Un cop enviat, preparem per capturar
                }
                
            }

            
            if (flagCAP && !SIO_isBusy()) {
                if(!SIO_isBusy()){
                    SIO_startCapture();
                    flagCAP = 0; 
                }
                
            }

            
            if (SIO_captureReady()) {
                char* captureData = SIO_getCapture();
                if(captureData[0] == 'Y' && captureData[1] == 'e' && captureData[2] == 's') {
                    estat = 6;
                } else if(captureData[0] == 'N' && captureData[1] == 'o') { 
                    estat = 8;
                } else {
                    flagR = 1; 
                }
            }
            break;

        case 6: // obrim les dues portes.
            if(!SIO_isBusy()) {
                SIO_sendString(MSG_OPEN_BOTH); 
                TI_ResetTics(ElTimer); 
                speaker_playAcuteSound();
                estat = 7;
            }
            
            
            break;

        case 7: //espera de 2 segons i tanquem les dues portes.
            
            if(!SIO_isBusy() && TI_GetTics(ElTimer) >= 1000) {
                SIO_sendString(MSG_CLOSE_BOTH); 
                TI_ResetTics(ElTimer); 
                Init_Controller();

            }
            break;

        case 8: // Estat d'Alarma.

            if(!SIO_isBusy()){

                SIO_sendString(MSG_THIEF);
                LED_ALARMA = 1; 
                LED_OK = 0;
                stopIntensity();
                speaker_playAlarmSound(); 
                    
            } 
            
            break;
        
        case 9:
        // un cop passats els 10 segons de l'alarma anirem al estat de reset i activarem el flag de reset per enviar el missatge i iniciar la captura
            if(TI_GetTics(ElTimer) >= 5000) { 
                TI_ResetTics(ElTimer); 
                flagR = 1; 
                flagCAP = 0;
                estat = 10; 
            }
            break;

        case 10: // estat de reset.
            
            //mostrem missatge.
            if (flagR && !SIO_isBusy()) {
                SIO_sendString(MSG_RESET); 
                flagR = 0; 
                flagCAP = 1; 
            }
            // iniciem captura.
            if (flagCAP && !SIO_isBusy()) {
                SIO_startCapture();
                flagCAP = 0; 
            }

            // gestionem resposta.
            if (SIO_captureReady()) {
                char* captureData = SIO_getCapture();
                if(captureData[0] == 'Y' && captureData[1] == 'e' && captureData[2] == 's') {
                    
                    Init_Controller();   
                } else {
                    flagR = 1; 
                }
            }
            break;
            
            
    }
}