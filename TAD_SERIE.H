#ifndef TAD_SERIAL_H
#define TAD_SERIAL_H

#include <xc.h>


#define HELLO_MESSAGE   "> LSBank - New Day!\r\n"
#define OPEN_EXT_MSG    "> LSBank - Open exterior door\r\n"
#define OPEN_INT_MSG    "> LSBank - Open interior door\r\n"
#define ASK_PIN_MSG     "> LSBandk - Enter PIN: "
#define CLOSE_EXT_MSG   "> LSBandk - Close exterior door\r\n"
#define CLOSE_INT_MSG   "> LSBandk - Close interior door\r\n"
#define OPEN_BOTH_MSG   "> LSBandk - Open booth doors\r\n"
#define CLOSE_BOTH_MSG  "> LSBandk - Close booth doors\r\n"
#define DEINED_MSG      "> LSBandk - Permission Denied\r\n"
#define THIEF_INT_MSG   "> LSBandk - Thief intercepted\r\n"
#define RESET_MSG       "> LSBandk - Reset System: "
#define EXT_REQUEST_MSG "> LSBandk - Exit Requested: "


void SERIAL_Init(void);
unsigned char Send_char_ok(unsigned char character);
unsigned char SERIAL_SendStr(const char *message);
void SERIAL_Motor(void);
unsigned char Serial_ResponsePending(void);
unsigned char Serial_GetResponse(void);


#endif