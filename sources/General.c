/*
 *
 * General.c
 *
 * Author: Motivo Engineering
 * Copyright 2014	
 *
 * Summary: General purpose and global routines
 *
 */

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "General.h"
#include "ISL94212.h"
#include "adc.h"
#include <math.h>
#include "uart.h"
//#include "my_can.h"


Uint8 GNR_ISRDepth=0;
Uint8 BalanceEnable[2];
Uint8 UnderVoltageLimit[2];
Uint8 OverVoltageLimit[2];
Uint16 NowCurrent=0;
Uint16 NowVoltage=0;
Parameters AllParameters;
Uint8 DeviceSetup[2];
Uint8 CellSetup[2];
Uint8 OverTempLimit[2];
Uint8 Reset[2];
SummaryFaults AllFaults;
ShelfArray OverTempAgg;
Uint8 Zero[2];
Uint8* NumCellsBalancing;

void DELAY_MS(Uint16 milliseconds) {

	while(milliseconds > 0) {

		DELAY_US(1000);
		milliseconds--;
	}
}
void DELAY_S(Uint8 seconds) {

	while(seconds > 0) {

		DELAY_MS(1000);
		seconds--;
	}
}

void EnableISR() {

	if(GNR_ISRDepth == 0) {

		EINT;
		return;
	}

	if(GNR_ISRDepth == 1) {

		GNR_ISRDepth = 0;
		EINT;
		return;
	}

	GNR_ISRDepth = GNR_ISRDepth - 1;
	return;
}

void DisableISR() {

	DINT;

	GNR_ISRDepth = GNR_ISRDepth + 1;
	return;
}

void ResetISR() {

	GNR_ISRDepth = 0;
	EINT;
	return;
}

void ToggleLED(Uint8 led) {

	if(led == 0) { GpioDataRegs.GPATOGGLE.bit.GPIO31 = 1; }
	if(led == 1) { GpioDataRegs.GPBTOGGLE.bit.GPIO34 = 1; }
}
#pragma CODE_SECTION(SetGPIO,".xtraCode")
void SetGPIO(Uint16 GPIOPin){
	if(GPIOPin>=32 && GPIOPin<=44){
		GpioDataRegs.GPBSET.all=1<<(GPIOPin-32);
	}
	else if(GPIOPin>=0 && GPIOPin <= 31){
		GpioDataRegs.GPASET.all=1<<GPIOPin;
	}
}

void ClearGPIO(Uint16 GPIOPin){
	if(GPIOPin>=32 && GPIOPin<=44){
		GpioDataRegs.GPBCLEAR.all=1<<(GPIOPin-32);
	}
	else if(GPIOPin>=0 && GPIOPin <= 31){
		GpioDataRegs.GPACLEAR.all=1<<GPIOPin;
	}
}

void LEDOn(LED Color){
	SetGPIO(Color);
}

void LEDOff(LED Color){
	ClearGPIO(Color);
}

#pragma CODE_SECTION(InitializeISLParameters,".bigCode")
void InitializeISLParameters(Uint8 NumDevices){
	Uint8 i=1;
	BalanceEnable[1]=0x1;
	BalanceEnable[0]=0x2;
	UnderVoltageLimit[1]=(Uint16)(((1<<13)/5)*1.05); 											//Set Lower Limit to 1.05
	UnderVoltageLimit[0]=(Uint16)(((1<<13)/5)*1.05)>>8;										//Set Lower Limit to 1.05
	OverVoltageLimit[1]=(Uint16)(((1<<13)/5)*1.67);											//Set Upper Limit to 1.67
	OverVoltageLimit[0]=(Uint16)(((1<<13)/5)*1.67)>>8;										//Set Upper Limit to 1.67
	DeviceSetup[0]=0x00;
	DeviceSetup[1]=0x80;																		//Dont  Measure while balancing
	CellSetup[1]=0x00;																			//Disable Cells 5,6,7 from faulting
	CellSetup[0]=0x00;
	OverTempLimit[1]=0x00;
	OverTempLimit[0]=0x38>>2;																	//Set overtemp to 55 Degrees C
	Reset[1]=0;
	Reset[0]=0;

    for(i=1;i<=NumDevices;i++){

    	ISL_WriteRegister(i,2,0x13, BalanceEnable); 											// This Initializes to Manual Mode and the Enable bit Set
													// Set to Scan Continuously
    	ISL_WriteRegister(i,2,0x11,UnderVoltageLimit); 											// Set UnderVoltage Limit
    	ISL_WriteRegister(i,2,0x10,OverVoltageLimit);											// Set OverVoltage Limit
    	ISL_WriteRegister(i,2,0x19,DeviceSetup);												// Disable Measure while balancing this doesn't work in manual mode
    	ISL_WriteRegister(i,2,0x05,CellSetup);													// Disable Cell 5,6,7 from faulting
    	ISL_WriteRegister(i,2,0x12,OverTempLimit);												// Set OverTemp to 55 Degrees C
    	ISL_WriteRegister(i,2,0x00,Reset);
    	ISL_WriteRegister(i,2,0x01,Reset);
    	ISL_WriteRegister(i,2,0x02,Reset);
    	ISL_Request(i, SCAN_CONTINOUS);
    	DELAY_S(1);

    }
}

void InitializeAllParameters(void){
	//DO EVERYTHING

}

Bool MakeABool(Uint8 ToBeMade){
	if(ToBeMade==0){
		return False;
	}
	else{
		return True;
	}
}

Parameters* GetParameters(void){
	return &AllParameters;
}
//So the point of checking the queue is to keep the bandwidth on the SPI Bus at the highest possible rate and not overflow it.
//This also leads to the CANBUS remaing at a fixed bandwidth this could increase if the SPI rate increased
//The result of this is that the more modules that are strung together the slower the update rate per module
#pragma CODE_SECTION(GetISLData,".bigCode")
void GetISLData(Uint8 NumDevices){
	Uint8 i;
	//NumCellsBalancing=GetCellsInBalanceOut();
	if(CPQ_Empty()==True){																		// Every loop check to see if the queue is empty
		if(*NumCellsBalancing>0){
		DELAY_MS(AllParameters.Balance.BleedResistorDelay);
		}
		for(i=1;i<=NumDevices;i++){																// Run through all of the devices
			Zero[0]=0x00;
			Zero[1]=0x00;
			ISL_Command(i,2,0x14,1,Zero,2, 0);
			ISL_Request(i,READ_TEMP);															// Read all of the temperatures
			ISL_Request(i,READ_VOLTAGES);														// Read all of the voltages from the devices
			ISL_Request(i,BALANCE_STATUS);														// Read all of the balance flags
			ISL_Request(i,READ_FAULTS);															// Read all of the fault flags

		}
	}

}

#pragma CODE_SECTION(GetMin,".bigCode")
Uint8 GetMin(Uint16* Array, Uint8 Length){
	Uint16 Min;
	Uint8 i;
	Uint8 MinIndex;
	i=0;
	Min=0xFFFF;
	for(i=0;i<Length;i++){
		if(Array[i]<Min){
			MinIndex=i;
			Min=Array[i];
		}
	}
	return MinIndex;
}
#pragma CODE_SECTION(GetMax,".xtraCode")
Uint8 GetMax(Uint16* Array, Uint8 Length){
	Uint16 Max;
	Uint8 i;
	Uint8 MaxIndex;
	i=0;
	Max=0;
	for(i=0;i<Length;i++){
		if(Array[i]>Max){
			MaxIndex=i;
			Max=Array[i];
		}
	}
	return MaxIndex;
}
#pragma CODE_SECTION(GetAvg,".xtraCode")
Uint16 GetAvg(Uint16* Array, Uint8 Length){
	Uint32 Sum;
	Uint16 Avg;
	Uint8 i;
	Avg=0;
	Sum=0;
	for(i=0;i<Length;i++){
		Sum+=Array[i];
	}
	Avg=Sum/Length;
	return Avg;
}

Uint16 GetNowCurrent(void){
	return NowCurrent;
}

Uint16 GetNowVoltage(void){
	return NowVoltage;
}

Uint16 GetCellsInBalance(void){
	Uint16 CurrentDeviceCells;
	ISL_DEVICE* ISLData;
	Uint8 i;
	Uint8 j;
	Uint16 CellsInBalance=0;
	for(i=0;i<NumDevices();i++){
		ISLData = GetISLDevices(i);
		CurrentDeviceCells=ISLData->PAGE2_2.SETUP.BSTAT.all;
		for(j=0;j<12;j++){
			CellsInBalance+=((CurrentDeviceCells>>j) & 1);
		}
	}
	return CellsInBalance;
}

SummaryFaults* CheckFaults(Uint8 device){
	ISL_DEVICE* ISLData = GetISLDevices(device);												//Get the Current Module
	if((ISLData->PAGE2_1.FAULT.OC.all & ~(7<<5))>0){
		AllFaults.OpenWire=True;
	}
	if((ISLData->PAGE2_1.FAULT.OF.all & ~(7<<5))>0){
		AllFaults.OverVoltage=True;
	}
	if((ISLData->PAGE2_1.FAULT.UF.all & ~(7<<5))>0){
		AllFaults.UnderVoltage=True;
	}
	if((ISLData->PAGE2_1.FAULT.OVTF.all)>0){
		AllFaults.OverTemp=True;
	}
}


// The balance cells function trips the balance resistors based on whether or not they are currently on and what the current voltage is
// when it trips it then checks to see if the voltage has fallen below the threshold minus the hysteresis 
#pragma CODE_SECTION(BalanceCells,".xtraCode")
void BalanceCells(Uint16 CurrentDevice){
	ISL_DEVICE* ISLData;
	Uint16 BalanceOut=0x0000;
	Uint8 BalanceChip[2];
	Uint16 stuff;
	Uint8 i;
	Uint16 BalanceThresholdOn=(((1<<13)/5)*1.64);																				//2^12/2.5*Number so 1.5 for now
	Uint16 BalanceThresholdOff=BalanceThresholdOn-(((1<<13)/5)*.05);															//2^12/2.5*Number so 1.45 for now
	Parameters* SystemParameters;
	SystemParameters= GetParameters();
	BalanceChip[0]=0x00;
	BalanceChip[1]=0x00;
	ISLData = GetISLDevices(CurrentDevice);
	BalanceOut=(*ISLData).PAGE2_2.SETUP.BSTAT.all;
	if((*SystemParameters).Balance.Enable==True){
		for (i=0;i<12;){
			if((*(((Uint16*)(&((*ISLData).PAGE2_2.SETUP.BSTAT.all))))&(1<<i))>0){
				if(*(((Uint16*)(&((*ISLData).PAGE1.CELLV.C1V)))+i)<((*SystemParameters).Balance.TopBalanceVoltage-(*SystemParameters).Balance.BalanceHysteresis)){
					BalanceOut &= ~(1<<i);
				}
				else{
					//Do Something
				}
			}
		else{
				if(*(((Uint16*)(&((*ISLData).PAGE1.CELLV.C1V)))+i)>(*SystemParameters).Balance.TopBalanceVoltage){
					BalanceOut |= (1<<i);
				}
				else{
					//Do Something
				}
			}
		i++;
			if(i==4){
				i=7;
			}
		}
	BalanceOut &= 0x0F8F;
	}
	else{
		BalanceOut=0;
	}
	if (BalanceOut==0){
		LEDOff(BLUE);
	}
	else{
		LEDOn(BLUE);
	}
	BalanceChip[0]=((BalanceOut>>8) & 0xFF);
	BalanceChip[1]=((BalanceOut) & 0x00FF);		//Bit Shift them to the right locations Chip 1 is 0-9
	//ISL_COMMAND deivce number is (2*Module)-1 for one and no minus 1 for the second
	ISL_Command(CurrentDevice+1,2,0x14,1,BalanceChip,2, 0);

}

#pragma CODE_SECTION(Setup,".bigCode")
void Setup() {
	Parameters* InitialParameters;

    // Initialize System Control:
    // PLL, WatchDog, enable Peripheral Clocks
    InitSysCtrl();

    // Step 2. Initialize GPIO:
    // This example function is found in the F2837xD_Gpio.c file and
    // illustrates how to set the GPIO to it's default state.
    //
    InitGpio(); // Skipped for this example

    DINT;
    // Initialize the PIE control registers to their default state.
    // The default state is all PIE interrupts disabled and flags
    // are cleared.
    InitPieCtrl();

    // Disable CPU interrupts and clear all CPU interrupt flags:
    IER = 0x0000;
    IFR = 0x0000;

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    // This will populate the entire table, even if the interrupt
    // is not used in this example.  This is useful for debug purposes.
    // The shell ISR routines are found in F2837xD_DefaultIsr.c.
    // This function is found in F2837xD_PieVect.c.
    //
    InitPieVectTable();

    // Enable PIE interrupts
    EALLOW;
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;
    EDIS;

    // Setup GPIO LED
	EALLOW;
	//GPIO Module Enable Pin
	GpioCtrlRegs.GPAMUX1.bit.GPIO0 = 0;										// 0=GPIO,  1=CANTX-A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO0 = 1;										// 1=OUTput,  0=INput
	GpioDataRegs.GPASET.bit.GPIO0 = 1;										// Set High initially


	//GPIO Fault Read Pin
	GpioCtrlRegs.GPAMUX1.bit.GPIO2 = 0;										// 0=GPIO,  1=CANTX-A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO2 = 0;										// 1=OUTput,  0=INput


	//GPIO BLUE LED
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;										// 0=GPIO,  1=CANTX-A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;										// 1=OUTput,  0=INput
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;										// Set Low

	//GPIO GREEN LED FOR CONTACTOR
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;										// 0=GPIO,  1=CANTX-A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;										// 1=OUTput,  0=INput
	GpioDataRegs.GPASET.bit.GPIO31 = 1;										// Set High initially

	//GPIO REDLED
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 0;										// 0=GPIO,  1=CANTX-A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO10 = 1;										// 1=OUTput,  0=INput
	GpioDataRegs.GPASET.bit.GPIO10 = 1;										// Set High initially

	EDIS;

	//------------------------------Initialize ADC------------------------//
	setup_adc();
	//------------------------------UART init-----------------------------//
	uart_init();
}


