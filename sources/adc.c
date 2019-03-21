/*
 * adc.c
 *
 *  Created on: Mar 14, 2019
 *      Author: Akshay Godase
 *      Description:
 *      This file handles all the initialization and handling of ADC interrupts
 */

#include "F28x_Project.h"
#include "adc.h"

//
// Globals
//
extern Uint16 NowCurrent;

void setup_adc()
{
    //
    // Map ISR functions
    //

    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr; //function for ADCA interrupt 1
    EDIS;

    //
    // Configure the ADC and power it up
    //
    ConfigureADC();

    //
    // Configure the ePWM
    //
    ConfigureEPWM();

    //
    // Setup the ADC for ePWM triggered conversions on channel 0
    //
    SetupADCEpwm(0);

    //
    // enable PIE interrupt
    //
    EALLOW;
    PieCtrlRegs.PIEIER1.bit.INTx1 = 1;
    IER |= M_INT1;
    EDIS;
    //
    // sync ePWM
    //
    EALLOW;
    CpuSysRegs.PCLKCR0.bit.TBCLKSYNC = 1;
    EDIS;
    //
    //start ePWM
    //
    EPwm5Regs.ETSEL.bit.SOCAEN = 1;  //enable SOCA
    EPwm5Regs.TBCTL.bit.CTRMODE = 0; //unfreeze, and enter up count mode


}

//
// ConfigureADC - Write ADC configurations and power up the ADC for both
//                ADC A and ADC B
//
void ConfigureADC(void)
{
    EALLOW;

    //
    //write configurations
    //
    AdcaRegs.ADCCTL2.bit.PRESCALE = 6; //set ADCCLK divider to /4
    AdcSetMode(ADC_ADCA, ADC_RESOLUTION_12BIT, ADC_SIGNALMODE_SINGLE);

    //
    //Set pulse positions to late
    //
    AdcaRegs.ADCCTL1.bit.INTPULSEPOS = 1;

    //
    //power up the ADC
    //
    AdcaRegs.ADCCTL1.bit.ADCPWDNZ = 1;

    //
    //delay for 1ms to allow ADC time to power up
    //
    DELAY_US(1000);

    EDIS;
}

//
// ConfigureEPWM - Configure EPWM SOC and compare values
//
void ConfigureEPWM(void)
{
    EALLOW;
    // Assumes ePWM clock is already enabled
    EPwm5Regs.TBCTL.bit.CLKDIV = 3;
    EPwm5Regs.TBCTL.bit.HSPCLKDIV = 6;
    EPwm5Regs.ETSEL.bit.SOCAEN    = 0;    // Disable SOC on A group
    EPwm5Regs.ETSEL.bit.SOCASEL    = 4;   // Select SOC on up-count
    EPwm5Regs.ETPS.bit.SOCAPRD = 1;       // Generate pulse on 1st event
    EPwm5Regs.CMPA.bit.CMPA = 0x0800;     // Set compare A value to 2048 counts
    EPwm5Regs.TBPRD = 0x1000;             // Set period to 4096 counts
    EPwm5Regs.TBCTL.bit.CTRMODE = 3;      // freeze counter
    EDIS;
}

//
// SetupADCEpwm - Setup ADC EPWM acquisition window
//
void SetupADCEpwm(Uint16 channel)
{
    Uint16 acqps;

    //
    // Determine minimum acquisition window (in SYSCLKS) based on resolution
    //
    if(ADC_RESOLUTION_12BIT == AdcaRegs.ADCCTL2.bit.RESOLUTION)
    {
        acqps = 14; //75ns
    }
    else //resolution is 16-bit
    {
        acqps = 63; //320ns
    }

    //
    //Select the channels to convert and end of conversion flag
    //
    EALLOW;
    AdcaRegs.ADCSOC0CTL.bit.CHSEL = channel;  //SOC0 will convert pin A0
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 5; //trigger on ePWM1 SOCA/C
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    EDIS;
}

//
// adca1_isr - Read ADC Buffer in ISR
//
interrupt void adca1_isr(void)
{
    NowCurrent= AdcaResultRegs.ADCRESULT0;
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// End of file
//

