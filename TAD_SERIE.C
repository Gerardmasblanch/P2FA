
#include "pic18f4321.h"
#include "TAD_SERIE.H"
#include <xc.h>

#define CONFIGURACIO_TXSTA 0x24
#define CONFIGURACIO_RCSTA 0x90
#define DIVISOR_BAUDRATE 64

unsigned char i = 0;
unsigned char mode = 0;
unsigned char receivedResponse[10]; // Variable para almacenar la respuesta recibida del usuario en modo de espera de respuesta (mode 2)
unsigned char *msg;
unsigned char dadesLlestes = 0;

void SIO_Init(void) {
    TRISCbits.TRISC6 = 1;
    TRISCbits.TRISC7 = 1;
    BAUDCONbits.BRG16 = 0;
    TXSTA = CONFIGURACIO_TXSTA;
    RCSTA = CONFIGURACIO_RCSTA;
    SPBRG = DIVISOR_BAUDRATE;
}



unsigned char SIO_RXAvail() {
    return ((PIR1bits.RCIF == 1) ? CERT : FALS);
}

unsigned char SIO_GetChar() {
    return RCREG;
}


unsigned char SIO_TXAvail(void) {
    return ((PIR1bits.TXIF == 1) ? CERT : FALS);
}

void SIO_PutChar (unsigned char Valor) {
    TXREG = Valor;
}

// cooperativitzar el motor SIO per a que es pugui enviar un missatge llarg sense bloquejar el sistema, per exemple, enviant un caràcter cada vegada que el motor sigui cridat i el buffer de transmissió estigui disponible.
void SIO_EnviarLog(const char* log) {

    if(TXSTAbits.TRMT){
        i = 0;
        msg = log; // Guardamos el mensaje a enviar en una variable global para que el motor pueda acceder a ella
        mode = 1; // Cambiamos a modo de envío de log

    }    
}
void SIO_modeResponse(void) {
    mode = 2; // Cambiamos a modo de espera de respuesta del usuario
    dadesLlestes = 0; // Reiniciamos el flag de datos listos para la nueva respuesta
}

unsigned char SIO_dadesLlestes(void) {
    return dadesLlestes;
}

unsigned char * SIO_getResponse(void) {
    return receivedResponse; // Retorna el último carácter recibido
}

// cooperativitzar el motor SIO per a que es pugui enviar un missatge llarg sense bloquejar el sistema, per exemple, enviant un caràcter cada vegada que el motor sigui cridat i el buffer de transmissió estigui disponible.
void SIO_Motor(void){
// mode 0 per enviar logs, mode 1 per esperar a que el missatge s'hagi enviat completament abans de permetre enviar un nou missatge
// mode 2 i els seguents per a rebre i enviar missatge escrit quan lusuari hagi de posar yes o no per al reset del sistema.
    switch (mode) {

        case 0:
            //estat de espera SIO no fa res. fa falta un flag que digui se es de enviar log o rebre missatge

            break;

        case 1:

            if (SIO_TXAvail()){
                SIO_PutChar(msg[i]);
                i++;
                if(msg[i] == '\0') {
                    mode = 0; // Cambiamos a modo de espera para el próximo mensaje
                    i = 0; // Reiniciamos el índice para la próxima vez que se envíe un mensaje
                }
            }
            break;
        

        case 2:

            if (SIO_RXAvail()) {
                receivedResponse[i] = SIO_GetChar();
                if(SIO_TXAvail()) {
                    SIO_PutChar(receivedResponse[i]); // Echo del carácter recibido
                    i++;
                }

                if(receivedResponse[i] == '\0') {
                    i = 0; // Reiniciamos el índice para la próxima respuesta
                    mode = 0; // Cambiamos a modo de espera para el próximo mensaje
                    dadesLlestes = 1; // Marcamos que los datos están listos para ser procesados
                }
                
            }
            
            break;
    }


}




