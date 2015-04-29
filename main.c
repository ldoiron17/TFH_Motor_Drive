

#include <stdint.h>        /* Includes uint16_t definition                    */
#include <stdbool.h>       /* Includes true/false definition                  */
#include <stdio.h>
#include "defines.h" //This used to hold global variables for my old senior project, get away from that. Stil useful for macros, #defines etc.
#include "init.h"
//#include "interrupts.h"//Useful I'm sure but not needed for LED blinking


//#include <pps.h> This came from the generic template from microchip for dspic code, not sure what it's use is...

//Initializs the oscillator registers (for some reason this can't go in the init_crystal() function, the compiler bitces)
_FOSCSEL( FNOSC_PRIPLL & IESO_OFF ); //Internal fast RC used on startup
_FOSC( POSCMD_XT & OSCIOFNC_OFF & FCKSM_CSDCMD ); //XT crystal oscllator mode w/out clock switching
_FWDT( FWDTEN_OFF ); //Watchdog timer disabled
_FICD( ICS_PGD1 & JTAGEN_OFF ); //JTAG disabled with PGEC1 and PGED1

int16_t main(void)
{

    initOSC();
    init_Ports();

	while(1){//Main loop
		LEDR_LAT = 1;
		LEDG_LAT = 1;
		LEDB_LAT = 0;
	}
}
