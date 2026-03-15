#ifndef TAD_CONTROLLER_H
#define TAD_CONTROLLER_H


void Init_Controller(void);
//Pre: Haver cridat aquesta funció abans de la resta
//Post: Inicialitza el sistema, posant els LEDs a l'estat inicial i reiniciant les variables i timers.   
void Hall_Ences(char ences);
//Pre: s'ha cridat Init_Controller.
//Post: si retorna 1 vol dir que s'ha detectat una polsació del sensor Hall.
void Pols_ExitRequest(char pols);
//Pre: s'ha cridat Init_Controller.
//Post: si retorna 1 vol dir que s'ha detectat una polsació del exit request.
void Motor_Controller(void);
//Pre: s'ha cridat Init_Controller.
//Post: Executa el motor de control el cual gestiona tota la lògica de la pràctica.
#endif
