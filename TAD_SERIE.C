#include "TAD_SERIE.H"
#include <xc.h>
#include "pic18f4321.h"

void SERIAL_Init(void){

    TRISCbits.TRISC6 = 1; // Pin Tx
    TRISCbits.TRISC7 = 1; // Pin Rx

    //Baudrate

    TXSTAbits.BRGH = 0;
    BAUDCONbits.BRG16 = 0;
    SPBRG = 64;  // 9600 baud aproximadament
    
    TXSTAbits.SYNC = 0;
    RCSTAbits.SPEN = 1;
    TXSTAbits.TXEN = 1;
    RCSTAbits.CREN = 1;

}

unsigned char Send_char_ok(unsigned char character){
    if (TXSTAbits.TRMT){
        TXREG = character;
        return 1;
    }
    return 0;
}

unsigned char SERIAL_SendStr(const char *missatge){
    unsigned char i = 0;
    if(missatge[i] != '\0'){
        if(Send_char_ok(missatge[i])){
            i++;
        }
        return 1;
    }else {
        return 0;
    }
}




void SERIAL_motor (void){
    unsigned char estat = 0;
    
    switch (estat){
        case 0:      
            break;
    }
}



