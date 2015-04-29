
#include <stdio.h>
#include <stdlib.h>
#include "init.h"
#include "defines.h"



volatile int BufferA[NUMSAMP] __attribute__((space(xmemory)));
volatile int BufferB[NUMSAMP] __attribute__((space(xmemory)));
volatile int BufferA2[NUMSAMP] __attribute__((space(xmemory)));
volatile int BufferB2[NUMSAMP] __attribute__((space(xmemory)));

void initOSC ( void )
{


	/*FOSC=FIN((M)/(N1*N2)*/
	//FP=FOSC/2, FIN=8MHz
	CLKDIVbits.PLLPRE = 0; // N1=2
	PLLFBD = 60; //M=60
	CLKDIVbits.PLLPOST = 0; // N2=2


	// Initiate Clock Switch to Primary Oscillator with PLL (NOSC=0b011)
	//    __builtin_write_OSCCONH(0x03);
	//    __builtin_write_OSCCONL(OSCCON | 0x01);
	//
	//    // Wait for clock switch to occur
	while ( OSCCONbits.COSC != 0b011 );

	// Wait for PLL to lock
	while ( OSCCONbits.LOCK != 1 );
}


//Functions:
//initAdc1() is used to configure A/D to convert AIN0 using CH0 and CH1 sample/hold in sequencial mode
//at 1.1MHz sample rate. ADC clock is configured at 13.3Mhz or Tad=75ns
void initAdc1(void)
{

        AD1CON1bits.ADON = 0;		// Turn off the A/D converter

        AD1CON1bits.FORM   = 0;		// Data Output Format: Signed Fraction (Q15 format)
        AD1CON1bits.SSRCG = 0;
        AD1CON1bits.SSRC   = 7;		// Interan Counter (SAMC) ends sampling and starts convertion (auto-convert if SSRCG=0)
        AD1CON1bits.ASAM   = 0;		// ADC Sample Control: Sampling begins immediately after conversion
        AD1CON1bits.AD12B  = 0;		// 10-bit ADC operation
        AD1CON1bits.SIMSAM = 1;         // Set up simulaneous sampling
        AD1CON2bits.CHPS  = 3;		// Converts CH0, CH1, CH2, and CH3

        AD1CON3bits.ADRC=0;		// ADC Clock is derived from Systems Clock
        AD1CON3bits.SAMC=1; 		// Auto Sample Time = 1*Tad
                // AD2CON3bits.ADCS=2 gives you a speed of aprrox. 212 ksps
        // AD2CON3bits.ADCS=2 gives you a speed of aprrox. 42 ksps
        AD1CON3bits.ADCS=25;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)=

        AD1CON1bits.ADDMABM = 1; 	// DMA buffers use in conversion order mode
        AD1CON2bits.SMPI    = 0;	// SMPI must be 0
        AD1CON4bits.ADDMAEN  = 1;	// Converts in ADC1BUF0


        /*NOTE: AN1 on the Rev0 of motion control board CANNOT be used
         Becaues the pin was lifted off of the PCB.  You must solder a
         jumper in order to use it! */
        AD1CHS123bits.CH123SA=0;	// //AN0 -> CH1, AN1 -> CH2, AN2 -> CH3,
        AD1CHS123bits.CH123NA=0;	// MUXA -ve input selection (Vref-) for CH1

        AD1CHS0bits.CH0NA = 0;          // Negative input for CH0 is vrefl
        AD1CHS0bits.CH0SA = 8;          //set AN8 -> CH0

        IFS0bits.AD1IF = 0;		// Clear the A/D interrupt flag bit
        IEC0bits.AD1IE = 0;		// Do Not Enable A/D interrupt

        AD1CON1bits.ADON = 1;		// Turn on the A/D converter
        AD1CON1bits.ASAM = 1;		// Sampling begins immediately after last conversion. SAMP bit is auto-set

}

//void initAdc2(void)
//{
//
//
//        AD2CON1bits.ADON = 0;		// Turn off the A/D converter
//
//        ANSELBbits.ANSB3=1;    //AN3 -> CH1 -> ADC3
//        ANSELBbits.ANSB4=1;    //AN4 -> CH2 -> ADC2
//        ANSELBbits.ANSB5=1;    //AN5 -> CH3 -> ADC1
//        ANSELBbits.ANSB9=1;    //AN9  -> CH0 -> ADC7
//
//
//        AD2CON1bits.FORM   = 0;		// Data Output Format: Signed Fraction (Q15 format)
//        AD2CON1bits.SSRCG = 0;
//        AD2CON1bits.SSRC   = 7;		// Interan Counter (SAMC) ends sampling and starts convertion (auto-convert if SSRCG=0)
//        AD2CON1bits.ASAM   = 0;		// ADC Sample Control: Sampling begins immediately after conversion
//        AD2CON1bits.SIMSAM = 1;         // Set up simulaneous sampling
//        AD2CON2bits.CHPS  = 3;		// Converts CH0, CH1, CH2, and CH3
//
//        AD2CON3bits.ADRC=0;		// ADC Clock is derived from Systems Clock
//        AD2CON3bits.SAMC=2; 		// Auto Sample Time = 0*Tad
//        // AD2CON3bits.ADCS=2 gives you a speed of aprrox. 212 ksps
//        AD2CON3bits.ADCS=2;		// ADC Conversion Clock Tad=Tcy*(ADCS+1)=
//                                        // ADC Conversion Time for 10-bit Tc=12*Tab =
//
//        AD2CON1bits.ADDMABM = 1; 	// DMA buffers are built in conversion order mode
//        AD2CON2bits.SMPI    = 0;	// SMPI must be 0
//        AD2CON4bits.ADDMAEN  = 1;	// Converts in ADC2BUF0
//
//        AD2CHS123bits.CH123SA=1;	//AN3 -> CH1, AN4 -> CH2, AN5 -> CH3,
//        AD2CHS123bits.CH123NA=0;	// MUXA -ve input selection (Vref-) for CH1
//
//        AD2CHS0bits.CH0NA = 0;          // Negative input for CH0 is vrefl
//        AD2CHS0bits.CH0SA = 9;          //set AN9 -> CH0
//
//        IFS1bits.AD2IF = 0;             // Clear the A/D interrupt flag bit
//        IEC1bits.AD2IE = 0;		// Do Not Enable A/D interrupt
//
//        AD2CON1bits.ADON = 1;		// Turn on the A/D converter
//
//        AD2CON1bits.ASAM = 1;		// Sampling begins immediately after last conversion. SAMP bit is auto-set
//
//}

void InitUART(void)
{
    U1MODEbits.UARTEN = 0;	// Bit15 TX, RX DISABLED, ENABLE at end of func
    U1MODEbits.USIDL = 0;	// Bit13 Continue in Idle
    U1MODEbits.IREN = 0;	// Bit11 No IR translation
    U1MODEbits.RTSMD = 0;	// Bit11 Flow Control Mode
    U1MODEbits.UEN = 0;         // Bits8,9 TX,RX enabled, CTS,RTS disabled
    U1MODEbits.WAKE = 0;	// Bit7 No Wake up (since we don't sleep here)
    U1MODEbits.LPBACK = 0;	// Bit6 No Loop Back
    U1MODEbits.ABAUD = 0;	// Bit5 No Autobaud (would require sending '55')
    U1MODEbits.URXINV = 0;	// Bit4 IdleState = 1  (for dsPIC)
    U1MODEbits.BRGH = 0;	// Bit3 16 clocks per bit period
    U1MODEbits.PDSEL = 0;	// Bits1,1 8bit, No Parity
    U1MODEbits.STSEL = 0;	// Bit0 One Stop Bit

    //U1BRG=(Fp/(16*Baud Rate))-1 where Fp=instruction clock cycle or FOSC/1
    //U1BRG=(60e6/(16*1504e3))-1
    //U1BRG=14 (exactly, no error!)
    U1BRG=14; //60MHz clock, 250e3 baud
    //U1BRG = 390; //9600  baud
    
    U1STAbits.UTXISEL1 = 0;	//Bit15 Interrupt when Char is transferred (1/1 config!)
    U1STAbits.UTXINV = 0;	//Bit14 U1TX Idle state is '0'
    U1STAbits.UTXISEL0 = 0;	//Bit13 Other half of Bit15
    U1STAbits.UTXBRK = 0;	//Bit11 Sync break disabled
    U1STAbits.UTXEN = 0;	//Bit10 TX pins controlled by periph
    U1STAbits.URXISEL = 0;	//Bits6,7 Int. on character recieved
    U1STAbits.ADDEN = 0;	//Bit5 Address Detect Disabled

    IFS0bits.U1TXIF = 0;	//Clear the Transmit Interrupt Flag
    IEC0bits.U1TXIE = 1;	//Enable Transmit Interrupts
    IFS0bits.U1RXIF = 0;	//Clear the Recieve Interrupt Flag
    IEC0bits.U1RXIE = 1;	//Enable Recieve Interrupts

    U1MODEbits.UARTEN = 1;	//Turn the peripheral on

    U1STAbits.UTXEN = 1;        //Transmit enabled


}

// DMA0 configuration
// Direction: Read from peripheral address 0-x300 (ADC1BUF0) and write to DMA RAM
// AMODE: Register indirect with post increment
// MODE: Continuous, Ping-Pong Mode
// IRQ: ADC Interrupt
// ADC stores results stored alternatively between DMA_BASE[0]/DMA_BASE[16] on every 16th DMA request

void initDma0(void)
{
	DMA0CONbits.AMODE = 0;			// Configure DMA for Register indirect with post increment
	DMA0CONbits.MODE  = 2;			// Configure DMA for Continuous Ping-Pong mode

	DMA0PAD=(int)&ADC1BUF0;
	DMA0CNT=(NUMSAMP-1);

	DMA0REQ=13; //Associate DMA with ADC1 ? ADC1 Convert Done
	#ifdef _HAS_DMA_
	DMA0STAL = __builtin_dmaoffset(&BufferA);
	DMA0STAH = __builtin_dmapage(&BufferA);

	DMA0STBL = __builtin_dmaoffset(&BufferB);
	DMA0STBH = __builtin_dmapage(&BufferB);
	#else
	DMA0STAL = (unsigned int)&BufferA;
	DMA0STAH = (unsigned int)&BufferA;

	DMA0STBL = (unsigned int)&BufferB;
	DMA0STBH = (unsigned int)&BufferB;
	#endif

	IFS0bits.DMA0IF = 0;			//Clear the DMA interrupt flag bit
        IEC0bits.DMA0IE = 1;			//Set the DMA interrupt enable bit

	DMA0CONbits.CHEN=1;


}

//void initDma1(void)
//{
//	DMA1CONbits.AMODE = 0;			// Configure DMA for Register indirect with post increment
//	DMA1CONbits.MODE  = 2;			// Configure DMA for Continuous Ping-Pong mode
//
//	DMA1PAD=(int)&ADC2BUF0;
//	DMA1CNT=(NUMSAMP-1);
//
//	DMA1REQ=21; //Associate DMA with ADC2
//	#ifdef _HAS_DMA_
//	DMA0STAL = __builtin_dmaoffset(&BufferA);
//	DMA0STAH = __builtin_dmapage(&BufferA);
//
//	DMA0STBL = __builtin_dmaoffset(&BufferB);
//	DMA0STBH = __builtin_dmapage(&BufferB);
//	#else
//	DMA1STAL = (unsigned int)&BufferA2;
//	DMA1STAH = (unsigned int)&BufferA2;
//
//	DMA1STBL = (unsigned int)&BufferB2;
//	DMA1STBH = (unsigned int)&BufferB2;
//	#endif
//
//	IFS0bits.DMA1IF = 0;			//Clear the DMA interrupt flag bit
//        IEC0bits.DMA1IE = 1;			//Set the DMA interrupt enable bit
//
//	DMA1CONbits.CHEN=1;
//
//
//}

void init_Ports(void){


/****Configure Analog Port pins (analog pins are ADC and Comparators pins)*****/

 /*Set used ADC anaolog pins to be Analog and Set all other pins to be digital*/

    //PORTA has no analog pins


     /*Normally AN0, AN1, AN2 and AN8 would be analog inputs, but I am not
     currently using them so that is why they are being set as digital outputs
     and being driven low in the unused pin section in this function*/

	//TODO: go through and update this file for this new processor, all these comments are from old senior project

    //PORTB
    _ANSB0 = 0;  //AN0 (used for ADC6 signal)
    _ANSB1 = 0;  //AN1 (used for ADC5 signal)
    _ANSB2 = 0;  //AN2 (used for ADC4 signal)
    _ANSB3 = 0;  //AN3 (used for ADC3 signal)
//    _ANSB4 = 1;  //AN4 (used for ADC2 signal)
//    _ANSB5 = 1;  //AN5 (used for ADC1 signal)
//    _ANSB6 = 0;  //PGEC1 (used for programming 5 pin header)
//    _ANSB7 = 0;  //PGED1 (used for programming 5 pin header)
    _ANSB8 = 0;  //AN8 (used for ADC8 signal)
//    _ANSB9 = 1;  //AN9 (used for ADC7 signal)
//    _ANSB10 = 0; //unused (AN10)
//    _ANSB11 = 0; //unused (AN11)
//    _ANSB12 = 0; //PWM8B (AN12)
//    _ANSB13 = 0; //PWM7B (AN13)
//    _ANSB14 = 0; //ENABLE7 (AN14)
//    _ANSB15 = 0; //PWM7A (AN15)



    //PORTC
    _ANSC1 = 0; //unused (AN16)
    _ANSC2 = 0; //Used as Limit Switch 1 (AN17)
//    _ANSC3 = 0; //Used as Limit Switch 2 (AN18)
//    _ANSC4 = 0; //Used as Limit Switch 3 (AN19)
//    _ANSC13 = 0; //Used as OSC1 pin, C3IN1- (comparator pin)
//    _ANSC14 = 0; //unused, C3IN1- (comparator pins)

    //PORTD
//    _ANSD6 = 0; //unused, C3IN2- (comparator pins)
//    _ANSD6 = 0; //unused, C3IN1+ (comparator pins)

    //PORTE
//    _ANSE0 = 0; //unused (AN24)
//    _ANSE1 = 0; //unused (AN25)
//    _ANSE2 = 0; //PWM1B (AN26)
//    _ANSE3 = 0; //ENABLE1 (AN27)
//    _ANSE4 = 0; //PWM1A (AN28)
//    _ANSE5 = 0; //TX1 (AN29)
//    _ANSE6 = 0; //RX1 (AN30)
//    _ANSE7 = 0; //unused (AN31)
//    _ANSE8 = 0; //unused (AN20)
//    _ANSE9 = 0; //unused (AN21)

    //PORTF has no Analog pins

    //PORT G
//    _ANSG6 = 0; //Limit Switch 4 (C1In3-)
//    _ANSG7 = 0; //Estop Switch (C1In1-)
//    _ANSG8 = 0; //LED1 (C2In3-)
//    _ANSG9 = 0; //LED2 Switch 4 (C2In1-)

/***************************Initialize UART1***********************************/

//    _U1RXR = 86; //RPI86 mapped to U1RX
//    _RP85R = 1; //RP85 mapped to U1TX
//    TRISEbits.TRISE5=0; //TX is output
//    TRISEbits.TRISE6=1; //RX is input

/************************Initialize RGB status LEDs******************************/
    LEDR_TRIS = 0; //Set Red LED pin as an output
    LEDG_TRIS = 0;	//Set Green LED pin as an output
    LEDB_TRIS = 0; //Set Blue LED pin as an output

	LEDR_LAT = 0;	//Initialize Red LED to be off
	LEDG_LAT = 0;	//Initialize Red LED to be off
	LEDB_LAT = 0;	//Initialize Red LED to be off


///***********Initialize Bit-banged "PWM" ports for gate driver inputs***********/
//
//    /*PWM1*/
//    TRISEbits.TRISE4 = 0; //Define PWM1A as output
//    TRISEbits.TRISE2 = 0; //Define PWM1B as output
//    TRISEbits.TRISE3 = 0; //Define ENABLE1 as output
//    PORTEbits.RE4 = 0; //Define PWM1A as output low
//    PORTEbits.RE2 = 0; //Define PWM1B as output low
//    PORTEbits.RE3 = 0; //Define ENABLE1 as output low
//
//    /*PWM2*/
//    TRISGbits.TRISG14 = 0; //Define PWM2A as output
//    TRISGbits.TRISG13 = 0; //Define PWM2B as output
//    TRISGbits.TRISG12 = 0; //Define ENABLE2 as output
//    PORTGbits.RG14 = 0; //Define PWM2A as output low
//    PORTGbits.RG13= 0; //Define PWM2B as output low
//    PORTGbits.RG12 = 0; //Define ENABLE2 as output low
//
//    /*PWM3*/
//    TRISCbits.TRISC13 = 0; //Define PWM3A as output
//    TRISDbits.TRISD11 = 0; //Define PWM3B as output
//    TRISDbits.TRISD0 = 0; //Define ENABLE3 as output
//    PORTCbits.RC13 = 0; //Define PWM3A as output low
//    PORTDbits.RD11 = 0; //Define PWM3B as output low
//    PORTDbits.RD0 = 0; //Define ENABLE3 as output low
//
//    /*PWM4*/
//    TRISAbits.TRISA3 = 0; //Define PWM4A as output
//    TRISDbits.TRISD10 = 0; //Define PWM4B as output
//    TRISAbits.TRISA4 = 0; //Define ENABLE4 as output
//    PORTAbits.RA3 = 0; //Define PWM4A as output low
//    PORTDbits.RD10 = 0; //Define PWM4B as output low
//    PORTAbits.RA4 = 0; //Define ENABLE4 as output low
//
//    /*PWM5*/
//    TRISAbits.TRISA2 = 0; //Define PWM5A as output
//    PORTAbits.RA2 = 0;    //Define PWM5A as output low
//    //   TRISGbits.TRISG3 = 0; //Define PWM5B as output  Can't use, input only (D+)
////    TRISGbits.TRISG2 = 0; //Define ENABLE5 as output Can't use, input only (D-)
//
//    /*PWM6*/
//    TRISFbits.TRISF3 = 0; //Define PWM6A as output
//    TRISFbits.TRISF8 = 0; //Define PWM6B as output
//    TRISFbits.TRISF2 = 0; //Define ENABLE6 as output
//    PORTFbits.RF3 = 0; //Define PWM6A as output low
//    PORTFbits.RF8 = 0; //Define PWM6B as output low
//    PORTFbits.RF2 = 0; //Define ENABLE6 as output low
//
//    /*PWM7*/
//    TRISBbits.TRISB15 = 0; //Define PWM7A as output
//    TRISBbits.TRISB13 = 0; //Define PWM7B as output
//    TRISBbits.TRISB14 = 0; //Define ENABLE7 as output
//     TRISBbits.TRISB15 = 0; //Define PWM7A as output
//    PORTBbits.RB13 = 0; //Define PWM7B as output low
//    PORTBbits.RB14 = 0; //Define ENABLE7 as output low
//
//    /*PWM8*/
//    TRISFbits.TRISF13 = 0; //Define PWM8A as output
//    TRISBbits.TRISB12 = 0; //Define PWM8B as output
//    TRISFbits.TRISF12 = 0; //Define ENABLE8 as output
//    PORTFbits.RF13 = 0; //Define PWM8A as output low
//    PORTBbits.RB12 = 0; //Define PWM8B as output low
//    PORTFbits.RF12 = 0; //Define ENABLE8 as output low
//
//
//    /*Initialize Limit switch inputs and Emergency Stop input*/
//
//    //Currently not using any limit switches so drive them as outputs at
//    //logical low
//    TRISCbits.TRISC2 = 0; //LIMIT SW1
//    PORTCbits.RC2 = 0; //Drive unsued pin low
//    TRISCbits.TRISC3 = 0; //LIMIT SW2
//    PORTCbits.RC3 = 0; //Drive unsued pin low
//    TRISCbits.TRISC4 = 0; //LIMIT SW3
//    PORTCbits.RC4 = 0; //Drive unsued pin low
//    TRISGbits.TRISG6 = 0; //LIMIT SW4
//    PORTGbits.RG6 = 0; //Drive unsued pin low
//    TRISGbits.TRISG7 = 0; //ESTOP
//    PORTGbits.RG7 = 0; //Drive unsued pin low
//
//    /*Set all unused pins to outputs and drive them low*/
//    /*Comment out the pins you are using in the following list*/
//
//    //PORT A
////    TRISAbits.TRISA0 = 0; //Define unused pin as an output pin
////    PORTAbits.RA0 = 0; //Drive unsued pin low
//    TRISAbits.TRISA1 = 0; //Define unused pin as an output pin
//    PORTAbits.RA1 = 0; //Drive unsued pin low
////    TRISAbits.TRISA2 = 0; //Define unused pin as an output pin
////    PORTAbits.RA2 = 0; //Drive unsued pin low
////    TRISAbits.TRISA3 = 0; //Define unused pin as an output pin
////    PORTAbits.RA3 = 0; //Drive unsued pin low
////    TRISAbits.TRISA4 = 0; //Define unused pin as an output pin
////    PORTAbits.RA4 = 0; //Drive unsued pin low
//    TRISAbits.TRISA5 = 0; //Define unused pin as an output pin
//    PORTAbits.RA5 = 0; //Drive unsued pin low
//    TRISAbits.TRISA6 = 0; //Define unused pin as an output pin
//    PORTAbits.RA6 = 0; //Drive unsued pin low
//    TRISAbits.TRISA7 = 0; //Define unused pin as an output pin
//    PORTAbits.RA7 = 0; //Drive unsued pin low
//    TRISAbits.TRISA9 = 0; //Define unused pin as an output pin
//    PORTAbits.RA9 = 0; //Drive unsued pin low
//    TRISAbits.TRISA10 = 0; //Define unused pin as an output pin
//    PORTAbits.RA10 = 0; //Drive unsued pin low
//    TRISAbits.TRISA14 = 0; //Define unused pin as an output pin
//    PORTAbits.RA14 = 0; //Drive unsued pin low
//    TRISAbits.TRISA15 = 0; //Define unused pin as an output pin
//    PORTAbits.RA15 = 0; //Drive unsued pin low
//
//    //PORT B
//    TRISBbits.TRISB0 = 0; //Define unused pin as an output pin
//    PORTBbits.RB0 = 0; //Drive unsued pin low
//    TRISBbits.TRISB1 = 0; //Define unused pin as an output pin
//    PORTBbits.RB1 = 0; //Drive unsued pin low
//    TRISBbits.TRISB2 = 0; //Define unused pin as an output pin
//    PORTBbits.RB2 = 0; //Drive unsued pin low
////    TRISBbits.TRISB3 = 0; //Define unused pin as an output pin
////    PORTBbits.RB3 = 0; //Drive unsued pin low
////    TRISBbits.TRISB4 = 0; //Define unused pin as an output pin
////    PORTBbits.RB4 = 0; //Drive unsued pin low
////    TRISBbits.TRISB5 = 0; //Define unused pin as an output pin
////    PORTBbits.RB5 = 0; //Drive unsued pin low
////    TRISBbits.TRISB6 = 0; //Define unused pin as an output pin
////    PORTBbits.RB6 = 0; //Drive unsued pin low
////    TRISBbits.TRISB7 = 0; //Define unused pin as an output pin
////    PORTBbits.RB7 = 0; //Drive unsued pin low
//    TRISBbits.TRISB8 = 0; //Define unused pin as an output pin
//    PORTBbits.RB8 = 0; //Drive unsued pin low
////    TRISBbits.TRISB9 = 0; //Define unused pin as an output pin
////    PORTBbits.RB9 = 0; //Drive unsued pin low
//    TRISBbits.TRISB10 = 0; //Define unused pin as an output pin
//    PORTBbits.RB10 = 0; //Drive unsued pin low
//    TRISBbits.TRISB11 = 0; //Define unused pin as an output pin
//    PORTBbits.RB11 = 0; //Drive unsued pin low
////    TRISBbits.TRISB12 = 0; //Define unused pin as an output pin
////    PORTBbits.RB12 = 0; //Drive unsued pin low
////    TRISBbits.TRISB13 = 0; //Define unused pin as an output pin
////    PORTBbits.RB13 = 0; //Drive unsued pin low
////    TRISBbits.TRISB14 = 0; //Define unused pin as an output pin
////    PORTBbits.RB14 = 0; //Drive unsued pin low
////    TRISBbits.TRISB15 = 0; //Define unused pin as an output pin
////    PORTBbits.RB15 = 0; //Drive unsued pin low
//
//
//    //PORTC
//    TRISCbits.TRISC1 = 0; //Define unused pin as an output pin
//    PORTCbits.RC1 = 0; //Drive unsued pin low
////    TRISCbits.TRISC2 = 0; //Define unused pin as an output pin
////    PORTCbits.RC2 = 0; //Drive unsued pin low
////    TRISCbits.TRISC3 = 0; //Define unused pin as an output pin
////    PORTCbits.RC3 = 0; //Drive unsued pin low
////    TRISCbits.TRISC4 = 0; //Define unused pin as an output pin
////    PORTCbits.RC4 = 0; //Drive unsued pin low
////    TRISCbits.TRISC12 = 0; //Define unused pin as an output pin
////    PORTCbits.RC12 = 0; //Drive unsued pin low
////    TRISCbits.TRISC13 = 0; //Define unused pin as an output pin
////    PORTCbits.RC13 = 0; //Drive unsued pin low
//    TRISCbits.TRISC14 = 0; //Define unused pin as an output pin
//    PORTCbits.RC14 = 0; //Drive unsued pin low
////    TRISCbits.TRISC15 = 0; //Define unused pin as an output pin
////    PORTCbits.RC15 = 0; //Drive unsued pin low
//
//    //PORTD
////    TRISDbits.TRISD0 = 0; //Define unused pin as an output pin
////    PORTDbits.RD0 = 0; //Drive unsued pin low
//    TRISDbits.TRISD1 = 0; //Define unused pin as an output pin
//    PORTDbits.RD1 = 0; //Drive unsued pin low
//    TRISDbits.TRISD2 = 0; //Define unused pin as an output pin
//    PORTDbits.RD2 = 0; //Drive unsued pin low
//    TRISDbits.TRISD3 = 0; //Define unused pin as an output pin
//    PORTDbits.RD3 = 0; //Drive unsued pin low
//    TRISDbits.TRISD4 = 0; //Define unused pin as an output pin
//    PORTDbits.RD4 = 0; //Drive unsued pin low
//    TRISDbits.TRISD5 = 0; //Define unused pin as an output pin
//    PORTDbits.RD5 = 0; //Drive unsued pin low
//    TRISDbits.TRISD6 = 0; //Define unused pin as an output pin
//    PORTDbits.RD6 = 0; //Drive unsued pin low
//    TRISDbits.TRISD7 = 0; //Define unused pin as an output pin
//    PORTDbits.RD7 = 0; //Drive unsued pin low
//    TRISDbits.TRISD8 = 0; //Define unused pin as an output pin
//    PORTDbits.RD8 = 0; //Drive unsued pin low
//    TRISDbits.TRISD9 = 0; //Define unused pin as an output pin
//    PORTDbits.RD9 = 0; //Drive unsued pin low
////    TRISDbits.TRISD10 = 0; //Define unused pin as an output pin
////    PORTDbits.RD10 = 0; //Drive unsued pin low
////    TRISDbits.TRISD11 = 0; //Define unused pin as an output pin
////    PORTDbits.RD11 = 0; //Drive unsued pin low
//    TRISDbits.TRISD12 = 0; //Define unused pin as an output pin
//    PORTDbits.RD12 = 0; //Drive unsued pin low
//    TRISDbits.TRISD13 = 0; //Define unused pin as an output pin
//    PORTDbits.RD13 = 0; //Drive unsued pin low
//    TRISDbits.TRISD14 = 0; //Define unused pin as an output pin
//    PORTDbits.RD14 = 0; //Drive unsued pin low
//    TRISDbits.TRISD15 = 0; //Define unused pin as an output pin
//    PORTDbits.RD15 = 0; //Drive unsued pin low
//
//    //PORTE
//    TRISEbits.TRISE0 = 0; //Define unused pin as an output pin
//    PORTEbits.RE0 = 0; //Drive unsued pin low
//    TRISEbits.TRISE1 = 0; //Define unused pin as an output pin
//    PORTEbits.RE1 = 0; //Drive unsued pin low
////    TRISEbits.TRISE2 = 0; //Define unused pin as an output pin
////    PORTEbits.RE2 = 0; //Drive unsued pin low
////    TRISEbits.TRISE3 = 0; //Define unused pin as an output pin
////    PORTEbits.RE3 = 0; //Drive unsued pin low
////    TRISEbits.TRISE4 = 0; //Define unused pin as an output pin
////    PORTEbits.RE4 = 0; //Drive unsued pin low
////    TRISEbits.TRISE5 = 0; //Define unused pin as an output pin
////    PORTEbits.RE5 = 0; //Drive unsued pin low
////    TRISEbits.TRISE6 = 0; //Define unused pin as an output pin
////    PORTEbits.RE6 = 0; //Drive unsued pin low
//    TRISEbits.TRISE7 = 0; //Define unused pin as an output pin
//    PORTEbits.RE7 = 0; //Drive unsued pin low
//    TRISEbits.TRISE8 = 0; //Define unused pin as an output pin
//    PORTEbits.RE8 = 0; //Drive unsued pin low
//    TRISEbits.TRISE9 = 0; //Define unused pin as an output pin
//    PORTEbits.RE9 = 0; //Drive unsued pin low
//
//    //PORTF
//    TRISFbits.TRISF0 = 0; //Define unused pin as an output pin
//    PORTFbits.RF0 = 0; //Drive unsued pin low
//    TRISFbits.TRISF1 = 0; //Define unused pin as an output pin
//    PORTFbits.RF1 = 0; //Drive unsued pin low
////    TRISFbits.TRISF2 = 0; //Define unused pin as an output pin
////    PORTFbits.RF2 = 0; //Drive unsued pin low
////    TRISFbits.TRISF3 = 0; //Define unused pin as an output pin
////    PORTFbits.RF3 = 0; //Drive unsued pin low
//    TRISFbits.TRISF4 = 0; //Define unused pin as an output pin
//    PORTFbits.RF4 = 0; //Drive unsued pin low
//    TRISFbits.TRISF5 = 0; //Define unused pin as an output pin
//    PORTFbits.RF5 = 0; //Drive unsued pin low
//    TRISFbits.TRISF8 = 0; //Define unused pin as an output pin
////    PORTFbits.RF8 = 0; //Drive unsued pin low
////    TRISFbits.TRISF12 = 0; //Define unused pin as an output pin
////    PORTFbits.RF12 = 0; //Drive unsued pin low
////    TRISFbits.TRISF13 = 0; //Define unused pin as an output pin
////    PORTFbits.RF13 = 0; //Drive unsued pin low
//
//    //PORTG
//    TRISGbits.TRISG0 = 0; //Define unused pin as an output pin
//    PORTGbits.RG0 = 0; //Drive unsued pin low
//    TRISGbits.TRISG1 = 0; //Define unused pin as an output pin
//    PORTGbits.RG1 = 0; //Drive unsued pin low
//    //G2 and G3 are input only pins
////    TRISGbits.TRISG6 = 0; //Define unused pin as an output pin
////    PORTGbits.RG6 = 0; //Drive unsued pin low
////    TRISGbits.TRISG7 = 0; //Define unused pin as an output pin
////    PORTGbits.RG7 = 0; //Drive unsued pin low
////    TRISGbits.TRISG8 = 0; //Define unused pin as an output pin
////    PORTGbits.RG8 = 0; //Drive unsued pin low
////    TRISGbits.TRISG9 = 0; //Define unused pin as an output pin
////    PORTGbits.RG9 = 0; //Drive unsued pin low
////    TRISGbits.TRISG12 = 0; //Define unused pin as an output pin
////    PORTGbits.RG12 = 0; //Drive unsued pin low
////    TRISGbits.TRISG13 = 0; //Define unused pin as an output pin
////    PORTGbits.RG13 = 0; //Drive unsued pin low
////    TRISGbits.TRISG14 = 0; //Define unused pin as an output pin
////    PORTGbits.RG14 = 0; //Drive unsued pin low
////    TRISGbits.TRISG15 = 0; //Define unused pin as an output pin
////    PORTGbits.RG15 = 0; //Drive unsued pin low



}

void initTimer1(void){  //Used for master timer for step speed

    T1CONbits.TON = 0; //Disable Timer1
    T1CONbits.TSIDL = 0; //Continues module operation in Idle mode
    T1CONbits.TGATE = 0; //Gated time accumulation is disabled
    T1CONbits.TCKPS = 0b11; //Timer1 Input Clock Prescale Select bits set to 1:256
    T1CONbits.TSYNC = 0;  //Ignored because TCS = 0
    T1CONbits.TCS = 0;  //Timer1 runs off of internal clock (Fp)

    PR1 =   400;
    //PR1 = 94; //Timer1 Compare Register timer overflow = (1/Fp)*PR1
    //Timer1 set to  100us when PR1 = 100 (10kHz)

    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TON = 1; //Enable Timer1
}

void initTimer2(void){ //Used for set off time of the current hysterisis

    T2CONbits.TON = 0; //Disable Timer1
    T2CONbits.TSIDL = 0; //Continues module operation in Idle mode
    T2CONbits.TGATE = 0; //Gated time accumulation is disabled
    T2CONbits.TCKPS = 0b11; //Timer1 Input Clock Prescale Select bits set to 1:64
    T2CONbits.T32 = 0;  //Set to a 16 bit timer
    T2CONbits.TCS = 0;  //Timer1 runs off of internal clock (Fp)  
    PR2 =  600; //Set period of timer
    IFS0bits.T2IF = 0;
    IEC0bits.T2IE = 1;
    T2CONbits.TON = 1; //Enable Timer2
}

int init_PWM1(void)
{
    PTCONbits.PTEN=0;  //Disable PWM

    PTCONbits.PTSIDL=0; //PWM runs in idle mode

//    PTCON2bits.PCLKDIV=6;   //Input clock prescaler divider
//    PTPER = 24000;//9615;      /* PTPER = ((2.5us) / 1.04ns) = 2404, where 2.5us*/
//    PTCONbits.SEVTPS = 0;  //special event generated on every compare match
//    SEVTCMP = 12000; // value to match master time base -> generate interupt
//    //PDC1=12000;    //50% duty cycle
//
//    PTCONbits.SEIEN = 1; // enable special event interupts

    
											
    PWMCON1bits.ITB = 1;    /* Use Phasex and SPHASEx registers for Independent time base */
    PWMCON1bits.DTC = 2;    /* Disable Deadtime */
    IOCON1bits.PENH = 0;    /* PWM1H is controlled by PWM module */
    IOCON1bits.PENL = 0;    /* PWM1L is controlled by PWM module */
    IOCON1bits.PMOD = 3;    /* Select True Independent Output PWM mode */

    PHASE1 = 10000;  /* PWM1H, PHASEx = ((1 / 500kHz) / 1.04ns) = 1923, where 500kHz */
    //SPHASE1 = 3840;  /* PWM1L, SPHASEx = ((1 / 250kHz) / 1.04ns) = 3846, where 250kHz
                   //   *  is the desired switching frequency and 1.04ns is PWM resolution. */
    PDC1 = 5000;             /* 50% Duty cycle for PWM1H */
   // SDC1 = 1920;            /* 50% Duty cycle for PWM1L */

//    PWMCON1bits.CAM=0;  //Edge-aligned mode enabled
//    PWMCON1bits.MDCS=0; //PDCx and SDCx control duty cycle
//    PWMCON1bits.CLIEN=0;    //Current limit interrupt disabled
//    PWMCON1bits.FLTIEN=0;   //Fault interrupt disabled

   // TRIG1bits.TRGCMP = 12000; //This is the compare value that triggers the ADC value

    IFS5bits.PWM1IF = 0;
    IEC5bits.PWM1IE = 1;
//    IFS3bits.PSEMIF = 0;
//    IEC3bits.PSEMIE = 1;
//    IPC14bits.PSEMIP = 1; /*assign priorty level, default is 4 on power up*/

    PTCONbits.PTEN=1;  //Enable PWM
}


