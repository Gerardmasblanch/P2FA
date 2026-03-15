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
    // Pre: haver cridat aquesta funció abans de qualsevol altra del TAD.
    // Post: Constructor del TAD SERIAL. Inicialitza la UART i deixa el mòdul en repòs.

void SIO_Motor(void);
    // Pre: Cal cridar aquesta funció periòdicament per gestionar l'enviament i la captura de dades.
    // Post: Executa el motor intern del TAD SERIAL.

void SIO_sendChar(unsigned char c);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Si el mòdul no està ocupat, comença l'enviament del caràcter 'c'.

void SIO_sendString(const char *str);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Si el mòdul no està ocupat, comença l'enviament de la cadena.

unsigned char SIO_isBusy(void);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Retorna cert si el mòdul està ocupat enviant o capturant dades, i fals altrament.

void SIO_startPin(void);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Inicia la captura d'un PIN de 7 caràcters enviats amb SIO_sendChar.

unsigned char SIO_pinReady(void);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Retorna cert si s'ha completat la captura del PIN, i fals altrament.

const char* SIO_getPIN(void);
    // Pre: SIO_pinReady ha retornat cert.
    // Post: Retorna un punter al buffer intern que conté el PIN capturat acabat en '\0'.

void SIO_startCapture(void);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Inicia la captura d'una línia rebuda per sèrie fins a '\r' o '\n'.

unsigned char SIO_captureReady(void);
    // Pre: El TAD ha estat inicialitzat amb SIO_Init.
    // Post: Retorna cert si s'ha completat la captura d'una línia, i fals altrament.

const char* SIO_getCapture(void);
    // Pre: SIO_captureReady ha retornat cert.
    // Post: Retorna un punter al buffer intern que conté la cadena capturada acabada en '\0'.

#endif