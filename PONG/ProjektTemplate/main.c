//----------------------------------------------------#GLOBAL#----------------------------------------------------------
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <pic32mx.h>
#include "header.h"
//-------------------------------------------------#MAIN_FUNCTIONS#-----------------------------------------------------

int main(void) {

    spi_init();                             //INITIALIZE SPI FOR DISPLAY
    display_init();                         //INITIALIZE DISPLAY
    init();                                 //INITIALIZE CLOCK AND INPUT/OUTPUT SIGNALS
    game_init();                            //INITIALIZE GAME

    resetScreen(arena);
    renderScreen(menu);

    enable_interrupt();                     //ENABLE INTERRUPT

    for(;;) {                               //RUN GAME "FOREVER" AND HANDLE INTERRUPT FUNCTIONS
	    return 0;
	}
}
