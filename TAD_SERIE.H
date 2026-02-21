#ifndef TAD_SERIE_H
#define TAD_SERIE_H


// --- DEFINICIÓ DE MISSATGES (LOGS) ---
// Pots afegir o modificar aquests missatges segons l'enunciat

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
#define FALS 0
#define CERT 1



void SIO_Init(void);

void SIO_Motor(void);

void SIO_EnviarLog(const char* log);

void SIO_modeResponse(void);

unsigned char SIO_dadesLlestes(void);

unsigned char *SIO_getResponse(void);
//pre: haver cridat a SIO_modeResponse() per posar el mode de espera de resposta del usuario,
// i haver rebut una resposta del usuario per SIO_RXAvail() i SIO_GetChar() que s'ha d'anar guardant 
//a receivedResponse fins que es rebi un enter o un altre caràcter que indiqui el final de la resposta,
// com un salt de línia o similar. Aquesta funció retorna un punter a receivedResponse, 
//on es pot accedir a la resposta completa rebuda del usuario.

unsigned char SIO_RXAvail(void);

unsigned char SIO_GetChar(void);

unsigned char SIO_TXAvail(void);

void SIO_PutChar(unsigned char val);

void SIO_Close(void);

#endif