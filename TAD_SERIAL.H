#ifndef TAD_SERIAL_H
#define TAD_SERIAL_H

#include <xc.h>

#define MSG_HELLO       "> LSBank - New Day!\r\n"
#define MSG_ASK_PIN     "> LSBank - Enter PIN: "
#define MSG_OPEN_EXT    "> LSBank - Open exterior door\r\n"
#define MSG_CLOSE_EXT   "> LSBank - Close exterior door\r\n"
#define MSG_OPEN_INT    "> LSBank - Open interior door\r\n"
#define MSG_CLOSE_INT   "> LSBank - Close interior door\r\n"
#define MSG_DENIED      "> LSBank - Permission Denied\r\n"
#define MSG_THIEF       "> LSBank - Thief Intercepted!\r\n"
#define MSG_RESET       "> LSBank - Reset System? "
#define MSG_EXIT_REQ    "> LSBank - Exit Requested: "
#define MSG_OPEN_BOTH   "> LSBank - Open both doors\r\n"
#define MSG_CLOSE_BOTH  "> LSBank - Close both doors\r\n"

void SIO_Init(void);
void SIO_Motor(void);

void SIO_sendChar(unsigned char c);
void SIO_sendString(const char *str);

void SIO_startPin(void);            
unsigned char SIO_pinReady(void);   
char* SIO_getPIN(void);             

void SIO_startCapture(void);            
unsigned char SIO_captureReady(void);   
char* SIO_getCapture(void);             

unsigned char SIO_isBusy(void);

#endif