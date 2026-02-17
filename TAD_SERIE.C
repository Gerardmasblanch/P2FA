
#include "pic18f4321.h"
#include "TAD_SERIE.H"
#include <xc.h>

// Estats de Transmissió (TX): 0 = Repòs, 1 = Enviant
static unsigned char estat_tx = 0;
static const char *ptr_tx = 0;
static unsigned char idx_tx = 0;

// Estats de Recepció (RX): 0 = Repòs (Sord), 1 = Escoltant
static unsigned char estat_rx = 0;
static char buffer_rx[10];
static unsigned char idx_rx = 0;
static unsigned char max_caracters = 0;

static unsigned char flag_dades_llestes = 0;

void SERIAL_Init(void) {
    TRISCbits.TRISC6 = 1; 
    TRISCbits.TRISC7 = 1; 
    TXSTAbits.BRGH = 0;
    BAUDCONbits.BRG16 = 0;
    SPBRG = 64; 
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;
}

// El Control activa l'enviament de missatges (Logs)
void SERIAL_EnviarLog(const char *msg) {
    if (estat_tx == 0) { // Només si no estem ja enviant un altre
        ptr_tx = msg;
        idx_tx = 0;
        estat_tx = 1; // Passem a estat ENVIANT
    }
}

// El Control ens diu: "Escolta el teclat fins a X caràcters"
void SERIAL_ActivarRecepcio(unsigned char quants) {
    idx_rx = 0;
    max_caracters = quants;
    flag_dades_llestes = 0;
    estat_rx = 1; // Passem a estat ESCOLTANT
}

// El Control ens pregunta: "Ja tenim el PIN?"
unsigned char SERIAL_DadesLlestes(void) {
    return flag_dades_llestes;
}

// El Control recull el resultat
char* SERIAL_GetBuffer(void) { 
    estat_rx = 0; // Tornem a estar Sords (IDLE)
    return buffer_rx; 
}

void SERIAL_Motor(void) {
    
    // --- MOTOR DE RECEPCIÓ (RX) ---
    switch (estat_rx) {
        case 0: // IDLE: El PIC ignora el que t'arribi per l'ordinador
            break;

        case 1: // ESCOLTANT: L'usuari està teclejant el PIN
            if (PIR1bits.RCIF) { 
                char c = RCREG;
                
                // Si l'usuari tecleja i no s'ha passat de llarg
                if (c != '\r' && idx_rx < max_caracters) {
                    buffer_rx[idx_rx] = c;
                    idx_rx++;
                    TXREG = c; // ECHO: Mostrem el caràcter al terminal
                }
                
                // Si premen Enter o arribem al límit (ex: 4 números)
                if (c == '\r' || idx_rx == max_caracters) {
                    buffer_rx[idx_rx] = '\0'; // Tanquem el text
                    flag_dades_llestes = 1;    // Avisem al Control
                    // Nota: estat_rx el posarà a 0 el Control quan cridi GetBuffer
                }
            }
            break;
    }

    // --- MOTOR DE TRANSMISSIÓ (TX) ---
    switch (estat_tx) {
        case 0: // IDLE: No hi ha res a dir
            break;

        case 1: // ENVIANT: Enviem el log caràcter a caràcter
            if (TXSTAbits.TRMT) { 
                TXREG = ptr_tx[idx_tx];
                idx_tx++;
                
                if (ptr_tx[idx_tx] == '\0') {
                    estat_tx = 0; // Tornem a IDLE quan acabem la frase
                }
            }
            break;
    }
}
