#include "TAD_SERIAL.H"
#include <xc.h>
#include "pic18f4321.h"

#define CONFIGURACIO_TXSTA 0x24
#define CONFIGURACIO_RCSTA 0x90
#define DIVISOR_BAUDRATE   64
#define SIZE_PIN 8
#define SIZE_CAPTURE 16


static unsigned char estat = 0;
static unsigned char carToSend = 0;
static const char* stringToSend = 0;

// PIN
static unsigned char pinFlag = 0;
static unsigned char pinPosition = 0;
static unsigned char pinCompleted = 0;
static unsigned char pinFlagSpace = 0;
static char pin[SIZE_PIN]; 

// CAPTURE
static unsigned char captureFlag = 0;
static unsigned char captureCompleted = 0;
static unsigned char capturePosition = 0;
static char capture[SIZE_CAPTURE];
static unsigned char c; 

void SIO_Init(void) {
    TRISCbits.TRISC6 = 0; // TX --> transmetre
    TRISCbits.TRISC7 = 1; // RX --> rebre

    BAUDCONbits.BRG16 = 0;
    TXSTA = CONFIGURACIO_TXSTA;
    RCSTA = CONFIGURACIO_RCSTA;
    SPBRG = DIVISOR_BAUDRATE;

    estat = 0;
    stringToSend = 0;
    carToSend = 0;

    // PIN
    pinFlag = 0;
    pinFlagSpace = 0;
    pinPosition = 0;
    pinCompleted = 0;
    pin[0] = '\0';

    // CAPTURE
    captureFlag = 0;
    captureCompleted = 0;
    capturePosition = 0;
    capture[0] = '\0';
}

void SIO_sendChar(unsigned char car) {
    if (estat != 0) return;   // si està ocupat, no accepta

    // logica per guardar el pin
    if (pinFlag && !pinCompleted) {
        
        if(pinPosition < 7) {
            pin[pinPosition] = car; //6
            pinPosition++;          //7
        }

        if (pinPosition >= 7) {     //7
            pin[7] = '\0'; 
            pinCompleted = 1;
            pinFlag = 0;
            pinPosition = 0;
            pinFlagSpace = 1;
        }
    }

    carToSend = car;          // guardar el byte
    estat = 1;                // enviant un caracter
}

void SIO_sendString(const char* str) {
    if (estat != 0) return;
    stringToSend = str;
    estat = 2;                // enviant un string
}


// PIN

void SIO_startPin(){
    pinFlag = 1;
    pinPosition = 0;
    pinCompleted = 0;
    pinFlagSpace = 0;
    pin[0] = '\0';
}

unsigned char SIO_pinReady(){
    return pinCompleted;
}

char* SIO_getPIN(){
    pinCompleted = 0;
    return pin;
}

// CAPTURE

void SIO_startCapture(){
    captureFlag = 1;
    captureCompleted = 0;
    capturePosition = 0;
    capture[0] = '\0';
    estat = 3;
}

unsigned char SIO_captureReady(){
    return captureCompleted;
}

char* SIO_getCapture(){
    captureCompleted = 0;
    return capture;
}

// MOTOR

void SIO_Motor(void) {

    switch (estat) {

        case 0: // en pausa
            break;

        case 1: // envia caracter
            if (!PIR1bits.TXIF) break; // flag si esta lliure o no per transmetre
            TXREG = carToSend; // pinPosition = 7 
            if (pinFlagSpace) {
                carToSend = '\n';
                estat = 1;
                pinFlagSpace = 0;
            } else {
                estat = 0; 
            }
            break;

        case 2: // enviant string
            if (!PIR1bits.TXIF) break;
            if (*stringToSend != '\0') {
                TXREG = *stringToSend;
                stringToSend++;
            } else {
                estat = 0;
                stringToSend = 0;
            }
            break;
        
        case 3: // captura
            if (!captureFlag) {
                estat = 0; 
                break; 
            }

            if (PIR1bits.RCIF) {
                c = RCREG;

                if (c == '\n') {
                    capture[capturePosition] = '\0';
                    captureCompleted = 1;
                    captureFlag = 0;
                    estat = 0;
                    capturePosition = 0;
                } else {
                    if (capturePosition < (sizeof(capture) - 1)) {
                        capture[capturePosition] = c;
                        capturePosition++;
                    } else {
                        capture[sizeof(capture) - 1] = '\0';
                        captureCompleted = 1;
                        captureFlag = 0;
                        estat = 0;
                        capturePosition = 0;
                    }
                }
            }
            break;

        default:
            estat = 0;
            break;
    }
}