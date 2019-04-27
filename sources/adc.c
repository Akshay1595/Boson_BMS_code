/*
 * adc.c
 *
 *  Created on: Mar 14, 2019
 *      Author: Akshay Godase
 *      Description:
 *      This file handles all the initialization and handling of ADC interrupts
 */

#include "all_header.h"

//
// Globals
//
extern Uint16 NowCurrent;
Uint16 tempValue16bit=0;
double current_in_amps = 0.00;
Uint16 OverCurrentThreshold = 4095;
double ambient_temp = 0.00;//ambient temperature

void setup_adc()
{
    //
    // Map ISR functions
    //

    EALLOW;
    PieVectTable.ADCA1_INT = &adca1_isr; //function for ADCA interrupt 1
    PieVectTable.ADCA2_INT = &adca2_isr; //function for ADCA interrupt 2
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
    PieCtrlRegs.PIEIER1.bit.INTx2 = 1;
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
    EPwm6Regs.ETSEL.bit.SOCAEN = 1; //enable SOCA
    EPwm6Regs.TBCTL.bit.CTRMODE = 0; //unfreeze, and enter up count mode


#ifdef DEBUG
    uart_string("ADC Setup Complete!\r\n");
#endif
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

    EPwm6Regs.TBCTL.bit.CLKDIV = 3;
    EPwm6Regs.TBCTL.bit.HSPCLKDIV = 6;
    EPwm6Regs.ETSEL.bit.SOCAEN = 0; // Disable SOC on A group
    EPwm6Regs.ETSEL.bit.SOCASEL = 4; // Select SOC on up-count
    EPwm6Regs.ETPS.bit.SOCAPRD = 1; // Generate pulse on 1st event
    EPwm6Regs.CMPA.bit.CMPA = 0x0800; // Set compare A value to 2048 counts
    EPwm6Regs.TBPRD = 0x1000; // Set period to 4096 counts
    EPwm6Regs.TBCTL.bit.CTRMODE = 3; // freeze counter


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
    AdcaRegs.ADCSOC1CTL.bit.CHSEL = 1; //SOC1 will convert pin A1
    AdcaRegs.ADCSOC0CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC1CTL.bit.ACQPS = acqps; //sample window is 100 SYSCLK cycles
    AdcaRegs.ADCSOC0CTL.bit.TRIGSEL = 13; //trigger on ePWM5 SOCA/C
    AdcaRegs.ADCSOC1CTL.bit.TRIGSEL = 15; //trigger on ePWM1 SOCA/C
    AdcaRegs.ADCINTSEL1N2.bit.INT1SEL = 0; //end of SOC0 will set INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT2SEL = 1; //end of SOC1 will set INT2 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT1E = 1;   //enable INT1 flag
    AdcaRegs.ADCINTSEL1N2.bit.INT2E = 1; //enable INT1 flag
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //make sure INT1 flag is cleared
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1; //make sure INT1 flag is cleared
    EDIS;
}

//
// adca1_isr - Read ADC Buffer in ISR
//
interrupt void adca1_isr(void)
{
    NowCurrent= AdcaResultRegs.ADCRESULT0;
    current_in_amps = convertToCurrent(NowCurrent+345);
    tempValue16bit = AdcaResultRegs.ADCRESULT1;
    ambient_temp = _ConvertTemperature(tempValue16bit);
    AdcaRegs.ADCINTFLGCLR.bit.ADCINT1 = 1; //clear INT1 flag
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

//
// adca2_isr - Read ADC Buffer in ISR
//
interrupt void adca2_isr(void)
{
    Uint16 value = AdcaResultRegs.ADCRESULT1;

    AdcaRegs.ADCINTFLGCLR.bit.ADCINT2 = 1; //clear INT1 flag
    //PieCtrlRegs.PIEACK.all = PIEACK_GROUP10;
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;

}

//Current ambient temperature in float value
double get_ambient_temperature(void)
{
    return ambient_temp;
}

//Current ambient temperature in 16bit value
Uint16 Now_amb_temp(void)
{
    return tempValue16bit;
}

//Convert 16bit value into temperature
double _ConvertTemperature(Uint16 tempValue)
{
    Uint16 offset = 1600;
    double Vin_by_Vo = (double) 4095/(double)tempValue;
    double r1 = 10 * 1000;
    double Rt = (r1 / (Vin_by_Vo - 1)) + offset;
    double logRt = log(Rt);
    double c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
    double Tc,T = (1.0 / (c1 + c2*logRt + c3*logRt*logRt*logRt));
    Tc = T - 273.15;
    return Tc;                                                                      // Use the Lookup table
}

//refer data sheet of current sensor
double convertToCurrent(Uint16 value)
{
    double mv = (double)value/4095.00 * 2400;
    double acoffset_mv = 1200.00; //
    double sensitivity = 12.00;
    current_in_amps = (mv - acoffset_mv) / sensitivity;
    return current_in_amps;
}
//
// End of file
//

