#include "TAD_KEYPAD.h"
#include "TAD_TIMER.h"
#include "TAD_SERIAL.h"
#include <xc.h>

#define KEYPAD_NINGUNA 20
#define TECLA_PREMUDA(!PORTBbits.RB3 || !PORTBbits.RB4 || !PORTBbits.RB5 || !PORTBbits.RB6)

static unsigned char timerSMS;
static unsigned char timerRebotes;

static unsigned char numClicksSMS = 0;
static unsigned char estat = 0;
static unsigned char columna = 0;
static unsigned char fila = 0;
static unsigned char posicioTecla = 0;

static unsigned char teclaPendiente = KEYPAD_NINGUNA; // 20 = ninguna
static unsigned char primerCiclo = 0;

static unsigned char m;

static const unsigned char keypad[4][3] = {
    {1, 2, 3},
    {4, 5, 6},
    {7, 8, 9},
    {10, 0, 11}
};

static const unsigned char keypadLectura[12][5] = {
    {'0',' ','0',' ','0'}, // 0 (2)
    {'1','1','1','1','1'}, // 1 (1)
    {'2','A','B','C','C'}, // 2 (3)
    {'3','D','E','F','F'}, // 3
    {'4','G','H','I','I'}, // 4
    {'5','J','K','L','L'}, // 5
    {'6','M','N','O','O'}, // 6
    {'7','P','Q','R','S'}, // 7 (4)
    {'8','T','U','V','V'}, // 8
    {'9','W','X','Y','Z'}, // 9 (4)
    {'*','*','*','*','*'}, // 10 (1)
    {'#','#','#','#','#'}  // 11 (1)
};

static void barridoPorts(unsigned char col) {
    LATBbits.LATB0 = 1;
    LATBbits.LATB1 = 1;
    LATBbits.LATB2 = 1;

    if(col == 0) LATBbits.LATB0 = 0;
    if(col == 1) LATBbits.LATB1 = 0;
    if(col == 2) LATBbits.LATB2 = 0;
}

static unsigned char getFila(void) {
    if(PORTBbits.RB3 == 0) return 0;
    if(PORTBbits.RB4 == 0) return 1;
    if(PORTBbits.RB5 == 0) return 2;
    return 3; // RB6
}

static unsigned char NumTeclaKeypad(unsigned char col, unsigned char fil) {
    if (TECLA_PREMUDA()) return keypad[fil][col];
    return KEYPAD_NINGUNA;
}

static char traduccioKeypad(unsigned char clicks, unsigned char tecla) {
    return keypadLectura[tecla][clicks];
}

static unsigned char maxClicks(unsigned char tecla){
    if (tecla == 1 || tecla == 10 || tecla == 11) return 1; // 1,*,#
    if (tecla == 0) return 2;                               // 0 
    if (tecla == 7 || tecla == 9) return 5;                 // 7 i 9
    return 4;                                               // 2-6-8
}

static void resetSMSState(void){
    numClicksSMS = 0;
    primerCiclo = 0;
}

static void commitPendiente(void){
    if (teclaPendiente == KEYPAD_NINGUNA) return;
    SIO_sendChar(traduccioKeypad(numClicksSMS, teclaPendiente));
    teclaPendiente = KEYPAD_NINGUNA;
    resetSMSState();
}

void KEY_Init(void){

    ADCON1bits.PCFG = 0x0F;

    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;

    TRISBbits.TRISB3 = 1;
    TRISBbits.TRISB4 = 1;
    TRISBbits.TRISB5 = 1;
    TRISBbits.TRISB6 = 1;

    INTCON2bits.RBPU = 0;

    TI_NewTimer(&timerRebotes);
    TI_NewTimer(&timerSMS);

    LATBbits.LATB0 = 1;
    LATBbits.LATB1 = 1;
    LATBbits.LATB2 = 1;

    estat = 0;
    teclaPendiente = KEYPAD_NINGUNA;
    resetSMSState();
}

void KEY_Motor(void){
    switch(estat){
        case 0:
            barridoPorts(columna = 0);
            if (primerCiclo && TI_GetTics(timerSMS) >= 500) { 
                commitPendiente();
                estat = 0;
            } else if (TECLA_PREMUDA()) {
                TI_ResetTics(timerRebotes);
                estat = 3;
            } else {
                estat = 1;
            }
            break;

        case 1:
            barridoPorts(columna = 1);
            if (primerCiclo && TI_GetTics(timerSMS) >= 500) {
                commitPendiente();
                estat = 0;
            } else if (TECLA_PREMUDA()) {
                TI_ResetTics(timerRebotes);
                estat = 3;
            } else {
                estat = 2;
            }
            break;

        case 2:
            barridoPorts(columna = 2);
            if (primerCiclo && TI_GetTics(timerSMS) >= 500) {
                commitPendiente();
                estat = 0;
            } else if (TECLA_PREMUDA()) {
                TI_ResetTics(timerRebotes);
                estat = 3;
            } else {
                estat = 0;
            }
            break;

        case 3: 
            if (TI_GetTics(timerRebotes) >= 9) {
                fila = getFila();
                posicioTecla = NumTeclaKeypad(columna, fila);
                estat = 4;
            } else {
                estat = 3;
            }
            break;

        case 4: 
            if (PORTBbits.RB3 && PORTBbits.RB4 && PORTBbits.RB5 && PORTBbits.RB6) {
                TI_ResetTics(timerRebotes);
                estat = 5;
            } else {
                estat = 4;
            }
            break;

        case 5: 
            if (TI_GetTics(timerRebotes) >= 9) {
                estat = 6;
            } else {
                estat = 5;
            }
            break;

        case 6: {

            if (posicioTecla == KEYPAD_NINGUNA) {
                estat = 0;
                break;
            }

            if (teclaPendiente != KEYPAD_NINGUNA && teclaPendiente != posicioTecla) {
                SIO_sendChar(traduccioKeypad(numClicksSMS, teclaPendiente));

                teclaPendiente = posicioTecla;
                numClicksSMS = 0;
                TI_ResetTics(timerSMS);
                primerCiclo = 1;

                if (maxClicks(teclaPendiente) == 1) {
                    SIO_sendChar(traduccioKeypad(0, teclaPendiente));
                    teclaPendiente = KEYPAD_NINGUNA;
                    resetSMSState();
                }
                estat = 0;
                break;
            }

            if (teclaPendiente == KEYPAD_NINGUNA) {
                teclaPendiente = posicioTecla;
                numClicksSMS = 0;
                TI_ResetTics(timerSMS);
                primerCiclo = 1;

                if (maxClicks(teclaPendiente) == 1) {
                    SIO_sendChar(traduccioKeypad(0, teclaPendiente));
                    teclaPendiente = KEYPAD_NINGUNA;
                    resetSMSState();
                }
                estat = 0;
                break;
            }

            m = maxClicks(teclaPendiente);
            if (m > 1) {
                numClicksSMS = (numClicksSMS + 1) % m;
                TI_ResetTics(timerSMS);
                primerCiclo = 1;
            } else {
                SIO_sendChar(traduccioKeypad(0, teclaPendiente));
                teclaPendiente = KEYPAD_NINGUNA;
                resetSMSState();
            }

            estat = 0;
            break;
        }

        default:
            estat = 0;
            break;
    }
}