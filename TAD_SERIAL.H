#ifndef TAD_SERIAL_H
#define TAD_SERIAL_H

#include <xc.h>

#define MSG_HELLO       "> LSBank - New Day!\r\n"
#define MSG_ASK_PIN     "> Enter PIN: "
#define MSG_OPEN_EXT    "> Open exterior door\r\n"
#define MSG_CLOSE_EXT   "> Close exterior door\r\n"
#define MSG_OPEN_INT    "> Open interior door\r\n"
#define MSG_CLOSE_INT   "> Close interior door\r\n"
#define MSG_DENIED      "> Permission Denied\r\n"
#define MSG_THIEF       "> THIEF INTERCEPTED!\r\n"
#define MSG_RESET       "> Reset System? "
#define MSG_EXIT_REQ    "> Exit Requested\r\n"

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

#endif