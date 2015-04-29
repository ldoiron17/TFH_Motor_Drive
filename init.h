/* 
 * File:   init.h
 * Author: Student
 *
 * Created on January 26, 2014, 3:49 PM
 */

#ifndef INIT_H
#define	INIT_H

#ifdef	__cplusplus
extern "C" {
#endif
#define NUMSAMP	4
    
extern volatile int BufferA[NUMSAMP] __attribute__((space(xmemory)));
extern volatile int BufferB[NUMSAMP] __attribute__((space(xmemory)));
extern volatile int BufferA2[NUMSAMP] __attribute__((space(xmemory)));
extern volatile int BufferB2[NUMSAMP] __attribute__((space(xmemory)));

extern void initOSC(void);    
extern void initAdc1(void);
extern void initAdc2(void);
extern void initDma0(void);
extern void initDma1(void);
void initTimer1(void);
void initTimer2(void);
void initUART(void);


#ifdef	__cplusplus
}
#endif

#endif	/* INIT_H */

