/*
 *
 * Timer.c
 *
 * Author: Motivo Engineering
 *
 * Summary: Simplified timer controls for f2803x family
 *
 * Copyright 2014	
 */

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "General.h"
#include "Timer.h"
//#include "CAN.h"

TMR_CALLBACKS TimerCallbacks;

void TMR_Init() {

	DisableISR();

	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.EPWM1_INT = &TMR_ISR1;
	PieVectTable.EPWM2_INT = &TMR_ISR2;
	EDIS;    // This is needed to disable write to EALLOW protected registers

	EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 0;      // Stop all the TB clocks
	EDIS;

	// Setup clock division // approximately 1uS per timer cycle
	EPwm1Regs.TBCTL.bit.CLKDIV    = 0x03; // divide by 32
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x06; // divide by 2
	                                     // total division is 32*2=64
	EPwm2Regs.TBCTL.bit.CLKDIV    = 0x03; // divide by 32
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x06; // divide by 2
	                                     // total division is 32*2=64
    // Setup Sync
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 0;  // Pass through
	EPwm2Regs.TBCTL.bit.SYNCOSEL = 0;  // Pass through

	// Initally disable Free/Soft Bits
	EPwm1Regs.TBCTL.bit.FREE_SOFT = 0;

	EPwm1Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;    // Count up
	EPwm1Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;      // Select INT on Zero event
	EPwm1Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;  // Enable INT
    EPwm1Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 1st event
    EPwm1Regs.TBCTR = 0x0001;					  // Set timer counter

    EPwm2Regs.TBCTL.bit.CTRMODE = TB_COUNT_UP;    // Count up
	EPwm2Regs.ETSEL.bit.INTSEL = ET_CTR_PRD;      // Enable INT on Zero event
	EPwm2Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE;  // Enable INT
	EPwm2Regs.ETPS.bit.INTPRD = ET_1ST;           // Generate INT on 1st event
	EPwm2Regs.TBCTR = 0x0001;					  // Set timer counter

	EALLOW;
	CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;        // Start all the timers synced
	EDIS;

	// Enable CPU INT3 which is connected to EPWM1-6 INT:
	IER |= M_INT3;



    // Enable EPWM INTn in the PIE: Group 3 interrupt 1-6
    PieCtrlRegs.PIEIER3.bit.INTx1 = PWM_INT_ENABLE;
    PieCtrlRegs.PIEIER3.bit.INTx2 = PWM_INT_ENABLE;
    PieCtrlRegs.PIEIER3.bit.INTx3 = PWM_INT_ENABLE;
    PieCtrlRegs.PIEIER3.bit.INTx4 = PWM_INT_ENABLE;
	EnableISR();
}

/**
void TMR_Test() {

	TMR_Init();

	TMR_SetTimeoutCallback(0, TMR_TestCallback);
	TMR_SetTimeout(0, 0x1FFF);

	TMR_StartTimer(0);

	DELAY_S(10);
}

void TMR_TestCallback() {

	Uint8 i;
	Uint8 j;
	for(i=0;i<256;i++) { j++; }
}
**/

Bool TMR_SetTimeoutCallback(Uint8 index, void (*callback)()) {

	if(index >= TMR_MAX_NUM_TIMERS) { return False; }

	TimerCallbacks.callback[index] = callback;

	return True;
}

Bool TMR_StartTimer(Uint8 index) {

	if(index >= TMR_MAX_NUM_TIMERS) { return False; }

	if(index == 0) {

		// Reset counter
		EPwm1Regs.TBCTR = 0x0001;

		// Clear pending events
		EPwm1Regs.ETPS.bit.INTPRD = 0;

		// Clear interrupt
		EPwm1Regs.ETCLR.bit.INT = 1;

		// Enable interrupt on first event
		EPwm1Regs.ETPS.bit.INTPRD=1;

		// Enable interrupts
		EPwm1Regs.ETSEL.bit.INTEN = PWM_INT_ENABLE;
	}
	else if(index == 1) {

		// Reset counter
		EPwm2Regs.TBCTR = 0x0001;

		// Clear pending events
		EPwm2Regs.ETPS.bit.INTPRD = 0;

		// Clear interrupt
		EPwm2Regs.ETCLR.bit.INT = 1;

		// Enable interrupt on first event
		EPwm2Regs.ETPS.bit.INTPRD=1;

		// Enable interrupts
		EPwm2Regs.ETSEL.bit.INTEN = PWM_INT_ENABLE;
	}

	return True;
}

Bool TMR_ResetTimer(Uint8 index) {

	if(index >= TMR_MAX_NUM_TIMERS) { return False; }

	if(index == 0) { EPwm1Regs.TBCTR = 0x0001; }
	if(index == 1) { EPwm2Regs.TBCTR = 0x0001; }

	return True;
}

Bool TMR_StopTimer(Uint8 index) {

	DisableISR();

	if(index >= TMR_MAX_NUM_TIMERS) { return False; }

	if(index == 0) { EPwm1Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE; }
	if(index == 1) { EPwm2Regs.ETSEL.bit.INTEN = PWM_INT_DISABLE; }

	EnableISR();

	return True;
}

// approximate
Bool TMR_SetTimeout(Uint8 index, Uint16 uS) {

	return TMR_SetTimeoutRegister(index, uS);
}

Bool TMR_SetTimeoutRegister(Uint8 index, Uint16 timerValue) {

	if(index >= TMR_MAX_NUM_TIMERS) { return False; }

	if(index == 0) { EPwm1Regs.TBPRD = timerValue; }
	if(index == 1) { EPwm2Regs.TBPRD = timerValue; }
	return True;
}

__interrupt void TMR_ISR1(void) {

	// Debug
	//ToggleLED(0);

	// Run callback
	(*(TimerCallbacks.callback[0]))();

    // Clear INT flag for this timer
    EPwm1Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}

__interrupt void TMR_ISR2(void) {

	// Debug
	//ToggleLED(1);

	// Run callback
	(*(TimerCallbacks.callback[1]))();

    // Clear INT flag for this timer
    EPwm2Regs.ETCLR.bit.INT = 1;
    // Acknowledge this interrupt to receive more interrupts from group 3
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;
}


