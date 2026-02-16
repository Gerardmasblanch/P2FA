#include <p18f4321.inc>
    CONFIG  OSC=HS     ; L?oscil.lador
    CONFIG  PBADEN=DIG ; Volem que el PORTB sigui DIGital
    CONFIG  WDT=OFF    ; Desactivem el WatchDog Timer

Comptador EQU 0x00

    ORG	    0x0000
    GOTO    MAIN 
    ORG	    0x0008
    RETFIE  FAST
    ORG	    0x0018
    RETFIE  FAST

ESPERA                  ; Procediment d?espera
    MOVLW   .8          ; W <- 8
    MOVWF   Comptador   ; Comptador <- W (Comptador := 8)
    NOP
    NOP
INCREMENTA
    INCF    Comptador,1 ; Comptador++
    BTFSS   STATUS,C,0  ; Status[Carry] == 1? 
    GOTO    INCREMENTA
    RETURN

MAIN
    SETF    ADCON1,0    ; Configurem el PORTA digital
    BCF     TRISA,3,0   ; Configurem el port RA3 de sortida
    
LOOP
    BSF	LATA,3,0    ;Encenem LED
    CALL    ESPERA      ;Rutina d'espera
    BCF	LATA,3,0    ;Apaguem LED 
    CALL    ESPERA      ;Rutina d'espera
    GOTO    LOOP

    END
