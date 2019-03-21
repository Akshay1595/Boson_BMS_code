/*
 *
 * Timer.h
 *
 * Author: Jama Mohamed
 *
 * Summary: Simplified timer controls for f2803x family
 *
 * Modified:
 *
 * 			 Added changelist   7/1/2014 JAM
 *
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "General.h"

#define TMR_MAX_NUM_TIMERS 4

#define PWM_INT_ENABLE   1
#define PWM_INT_DISABLE  0

typedef struct {
	void (*callback[TMR_MAX_NUM_TIMERS])();
} TMR_CALLBACKS;

void TMR_Init();
void TMR_Test();
void TMR_TestCallback();

Bool TMR_SetTimeoutCallback(Uint8 index, void (*callback)());
Bool TMR_StartTimer(Uint8 index);
Bool TMR_ResetTimer(Uint8 index);
Bool TMR_StopTimer(Uint8 index);
Bool TMR_SetTimeout(Uint8 index, Uint16 uS);
Bool TMR_SetTimeoutRegister(Uint8 index, Uint16 timerValue);

__interrupt void TMR_ISR1(void);
__interrupt void TMR_ISR2(void);
__interrupt void TMR_ISR3(void);
__interrupt void TMR_ISR4(void);

#endif /* TIMER_H_ */
