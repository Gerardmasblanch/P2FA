#include "pic18f4321.h"
#include "TAD_TIMER.H"
#include "TAD_CONTROLLER.H"
#include "TAD_INTENSITY.h"
#include "TAD_SERIAL.H"
#include "TAD_KEYPAD.h"
//#include "TAD_HALL.H"
//#include "TAD_SPEAKER.H"
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


void Init_Controller(){
    // sortida els pins dels leds
    TRISAbits.RA2 = 0;
    TRISAbits.RA3 = 0;
    
    // posem en 1 el led ok i apagem led alarma i reiniciem intents i l'estat
    LED_OK = 1;
    LED_ALARMA = 0;
    intents = 0;
    estat = 0;

    flagCAP = 0;

    TI_NewTimer(&ElTimer); 
    TI_NewTimer(&timerNewDay);
    TI_ResetTics(timerNewDay);

}

//void Hall_Ences(char ences) {
//    if (ences) {
 //       pols_hall = 1; // hall activat
 //   } 
//}

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
                
                SIO_sendString(MSG_THIEF); // Enviem log de "THIEF INTERCEPTED!"
                LED_ALARMA = 1; // Encenem el led d'alarma
                //apagar led intesity
                //so altaveu alarma
                estat = 5;   // Aixo tampoc va aqui
                LED_OK = 0;
                stopIntensity();

                flagCAP = 1; // de moment pero no va aqui

                
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
                SIO_sendString(MSG_CLOSE_INT); // Enviem log de tancament de porta interior
            }
            break;
        
        case 4: // porta interior tancada, sistema en espera de nova activació
            if(pol_exit_request == 1) {   
                SIO_sendString(MSG_EXIT_REQ);
                SIO_startCapture(); // Iniciem la captura de dades per al log d'Exit Request
                estat = 5; // canviem a l'estat d'espera de captura
            } 
            break;
        case 5: 

            if (flagCAP && !SIO_isBusy()) {
                SIO_startCapture();
                if (flagCAP && !SIO_isBusy()) {
                    SIO_sendString(MSG_EXIT_REQ); 
                    flagCAP = 0; 
                }
            }
            
            if (SIO_captureReady()) {
                    char* captureData = SIO_getCapture();
                    SIO_sendString(captureData); // Enviem el log d'Exit Request amb les dades capturades
                    if(captureData[0] == 'Y' && captureData[1] == 'E' && captureData[2] == 'S') { // Si la captura indica que es confirma el reset del sistema
                        
                    } else {
                        if(captureData[0] == 'N' && captureData[1] == 'O') { // Si la captura indica que no es confirma el reset del sistema
                            estat = 8;
                        } else {
                            SIO_sendString(MSG_RESET); // Enviem log de "Reset System?" per si la captura no és clara o no conté una resposta vàlida
                        }
                        estat = 6;
                    }
                } else {
                    estat = 4; // Esperem a que la captura estigui llesta
                }
            
            
    }
}