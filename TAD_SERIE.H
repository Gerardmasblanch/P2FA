#ifndef TAD_SERIE_H
#define TAD_SERIE_H

#include <xc.h>

// --- DEFINICIÓ DE MISSATGES (LOGS) ---
// Pots afegir o modificar aquests missatges segons l'enunciat
#define MSG_HELLO       "\r\n> LSBank - New Day!\r\n"
#define MSG_ASK_PIN     "> Enter PIN: "
#define MSG_OPEN_EXT    "> Open exterior door\r\n"
#define MSG_CLOSE_EXT   "> Close exterior door\r\n"
#define MSG_OPEN_INT    "> Open interior door\r\n"
#define MSG_CLOSE_INT   "> Close interior door\r\n"
#define MSG_DENIED      "> Permission Denied\r\n"
#define MSG_THIEF       "> THIEF INTERCEPTED!\r\n"
#define MSG_RESET       "> Reset System? (Y/N): "
#define MSG_EXIT_REQ    "> Exit Requested\r\n"

// --- FUNCIONS PÚBLIQUES ---

// Inicialitza el hardware EUSART (9600 baud, etc.)
void SERIAL_Init(void);

// El motor que s'ha de cridar al while(1)
void SERIAL_Motor(void);

// Envia una cadena de caràcters (logs) de forma cooperativa
void SERIAL_EnviarLog(const char *msg);

// Activa la recepció de caràcters. 
// 'quants' defineix el límit (ex: 4 per al PIN, 1 per al Yes/No)
void SERIAL_ActivarRecepcio(unsigned char quants);

// Retorna 1 si el buffer ja té la informació completa (PIN o caràcter de reset)
unsigned char SERIAL_DadesLlestes(void);

// Retorna el punter a la memòria on s'ha guardat el PIN/caràcter
char* SERIAL_GetBuffer(void);

#endif