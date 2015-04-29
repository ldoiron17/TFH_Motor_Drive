/* 
 * File:   defines.h
 * Author: Lance Doiron
 *
 * Created on April 28, 2015, 6:22 PM
 */

#ifndef DEFINES_H
#define	DEFINES_H

/* Device header file */
#if defined(__XC16__)
    #include <xc.h>
#elif defined(__C30__)
    #if defined(__dsPIC33E__)
    	#include <p33Exxxx.h>
    #elif defined(__dsPIC33F__)
    	#include <p33Fxxxx.h>
    #endif
#endif


#ifdef	__cplusplus
extern "C" {
#endif

#define LEDR_TRIS TRISDbits.TRISD8
#define LEDG_TRIS TRISDbits.TRISD6
#define LEDB_TRIS TRISCbits.TRISC13

#define LEDR_LAT LATDbits.LATD8
#define LEDG_LAT LATDbits.LATD6
#define LEDB_LAT LATCbits.LATC13


#ifdef	__cplusplus
}
#endif

#endif	/* DEFINES_H */

