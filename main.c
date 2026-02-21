/*
 * File:   main.c
 * Author: gerim
 *
 * Created on 16 de febrero de 2026, 17:31
 */

 // --- INCLUDES --- de tots els TADs 

#include "TAD_SERIE.H" 
#include "TAD_EXITREQUEST.H"
#include "TAD_CONTROLLER.H"
#include "TAD_HALL.H"
#include "TAD_SPEAKER.H"
#include "TAD_KEYPAD.H"
#include "TAD_INTESITY.H"
#include "TAD_TIMER.H"
#include <xc.h>


void main(void) {
    
    //inits de tots els TADs
    SIO_Init();
    EXITREQUEST_Init();
    TIMER_Init();
    CONTROLLER_Init();

    while(1){

        SIO_Motor();
        EXITREQUEST_Motor();
        CONTROLLER_Motor();
        HALL_Motor();
        SPEAKER_Motor();
        KEYPAD_Motor();
        INTENSITY_Motor();

    }
    
    
    return;
}
