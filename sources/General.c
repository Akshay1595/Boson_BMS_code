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

#include "all_header.h"
//
// Globals
//
extern Uint8 FailCounter;
Uint8 NumISLDevices=0x00;
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

#ifndef _FLASH
#pragma CODE_SECTION(InitializeISLParameters,".bigCode")
#endif

void InitializeISLParameters(Uint8 NumDevices){
	Uint8 i=1;
	Uint8 buf[16] = {};
	Uint8 Fault_setup[2] = {};
	BalanceEnable[1]=0x1;
	BalanceEnable[0]=0x2;
	DeviceSetup[0]=0x00;
	DeviceSetup[1]=0x80;																		//Dont  Measure while balancing
	Uint8 disable_cell_array[12] = {0,0,0,0, 1,1,1,1, 1,0,0,0};                                  // we dont want to disable any cell hence we are keeping
	                                                                                            // all zeros if cell1 we want to disable arr[0] = 1
	Fault_setup[1]  = 0x60; //default tot0 and tot1 to 11 which takes 8 sample at scan interval of 16ms
	Fault_setup[0]  = 0x1F; //enable external temperature devices to fault

	OverTempLimit[1]=0x9D;
	OverTempLimit[0]=0x0C;																	//Set overtemp to 55 Degrees C
	Reset[1]=0;
	Reset[0]=0;

#ifdef DEBUG
	uart_string("UV Limit set to = ");
	float_to_ascii(UV_LIMIT, buf);
	uart_string(buf);
	uart_string_newline("OV_limit set to = ");
	float_to_ascii(OV_LIMIT, buf);
    uart_string(buf);
    uart_string_newline("OVTF limit set to = ");
    my_itoa(OT_LIMIT, buf);
    uart_string(buf);
    uart_string("degreeC\r\n");
#endif

    for(i=1;i<=NumDevices;i++){
        set_over_temperature_limit(i,OT_LIMIT);
        ISL_WriteRegister(i,2,0x03, Fault_setup);                                               // allow external temperature measurement to set fault bits
    	ISL_WriteRegister(i,2,0x13, BalanceEnable); 											// This Initializes to Manual Mode and the Enable bit Set
    	write_undervoltage_threshold(i,UV_LIMIT);                                                   //sets under_voltage threshold
    	write_overvoltage_threshold(i, OV_LIMIT);                                                   //sets over_voltage threshold
    	ISL_WriteRegister(i,2,0x19,DeviceSetup);												// Disable Measure while balancing this doesn't work in manual mode
    	disable_cell_from_faulting(i, disable_cell_array);                                      //disable some cells from faulting
    	ISL_WriteRegister(i,2,0x00,Reset);                                                      //reset all the pages one by one
    	ISL_WriteRegister(i,2,0x01,Reset);
    	ISL_WriteRegister(i,2,0x02,Reset);
    	ISL_Request(i, SCAN_CONTINOUS);                                                         // Set to Scan Continuously
    	ISL_WriteRegister(i, 2, 0x04, Reset);                                     //clear fault status register
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
#ifndef _FLASH
#pragma CODE_SECTION(GetISLData,".bigCode")
#endif

void GetISLData(Uint8 NumDevices){
	Uint8 i;
	//NumCellsBalancing=GetCellsInBalanceOut();
	checkForCommFailure();                                                                      //check if communication failure is there
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

#ifndef _FLASH
#pragma CODE_SECTION(GetMin,".bigCode")
#endif
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
#ifndef _FLASH
#pragma CODE_SECTION(GetMax,".bigCode")
#endif
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
#ifndef _FLASH
#pragma CODE_SECTION(GetAvg,".bigCode")
#endif
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

#ifndef _FLASH
#pragma CODE_SECTION(Setup,".bigCode")
#endif
void Setup() {
#ifdef _FLASH
    memcpy(&RamfuncsRunStart, &RamfuncsLoadStart, (size_t)&RamfuncsLoadSize);
#endif

    DisableISR();

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

	//GPIO GREEN LED FOR CONTACTOR
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 0;										// 0=GPIO,  1=CANTX-A,  2=Resv,  3=Resv
	GpioCtrlRegs.GPADIR.bit.GPIO31 = 1;										// 1=OUTput,  0=INput
	GpioDataRegs.GPASET.bit.GPIO31 = 1;										// Set High initially

	EDIS;

    //------------------------------UART init-----------------------------//
    uart_init();
#ifdef DEBUG
    uart_string("Initially Turning Off Contactor....!\r\n");
#endif
    contactor_off();

	//------------------------------Initialize ADC------------------------//
    contactor_gpio_setup();
	setup_adc();
    TMR_Init();
    SPI_Init();
    can_init();
    COMMLEDSetup();
    COMLEDOff();
    ConfigureFaultSetup();
    init_alert_task();
    //SPI_Test();
    Bool Did_it_blend;

    Did_it_blend = ISL_Init_Retry(2);

    if (Did_it_blend == True)
    {
        COMLEDOn();
#ifdef DEBUG
        uart_string("Yes it has connected Successfully!\r\n");
#endif
    }
    else
    {
#ifdef DEBUG
        uart_string("I couldn't find ISL devices!\r\n");
#endif
        FaultLEDOn();
        while(1);
    }

    ResetISR();

    NumISLDevices=NumDevices();
    ISL_EnableReceiveCallback();                                            // Enable the recieve call back
    ISL_SetReceiveCallback(RecieveHandler);                                 // Set the recievehandler to call when data is recieved from the daisy chain

    Uint8 _buf[8] = {};
#ifdef DEBUG
    uart_string("There are ");
#endif
    my_itoa(NumISLDevices, _buf);
    uart_string(_buf);
#ifdef DEBUG
    uart_string(" devices!\r\n");
#endif

#ifdef DEBUG
    uart_string("Setup Complete!\r\n");
#endif

    InitializeISLParameters(NumISLDevices);                                 // Initialize the default values into the ISL Registers

    GetISLData(NumISLDevices);                                              //read before turning On Contactor
    DELAY_S(2);
    fault_isr();
    contactor_on();
    DELAY_S(1);


#ifdef DEBUG
    #ifdef PARTIAL_LOG
    uart_string(",,Device1,,,,,,Device2,,,,,,Device3,,,,,,Device4,,,,,\r\n");
    uart_string("Vcmax,Vcmin,Vpack,Tcmin,Tcmax,,Vcmax,Vcmin,Vpack,Tcmin,Tcmax,,Vcmax,Vcmin,Vpack,Tcmin,Tcmax,,Vcmax,Vcmin,Vpack,Tcmin,Tcmax,AmbTemp,SOC,Vbat\r\n");
    #endif
#endif
}

//
//This function checks if communication failure is there and if there it calls handle for it
//
void checkForCommFailure(void)
{
    ISL_FLAGS* RecieveFlags;
    RecieveFlags=GetISLFlags();
    if( RecieveFlags->timeout == True || (RecieveFlags->newAck == False && RecieveFlags->newData == False ))
    {
        // If we do not get any data out of ISL devices raise a bug and say that it has failed.. Let him reconnect
        FailCounter++;
    }
    if ( FailCounter > 4 )
    {
        handle_comm_failure();
    }
}

//
// This code is to handle the Communication failure
//
void handle_comm_failure(void)
{
    //keeping fault counter = 0 to restart
    FailCounter = 0;
    //disable recv_call back first since there is no need to send continuous CAN message now
    CPQ_Flush();
    ISL_DisableReceiveCallback();
    FaultLEDOn();
#ifdef DEBUG
    uart_string("Communication has failed\r\n");
    uart_string("Retrying...\r\n");
#endif
    while(ISL_Init_Retry(2) == False);
#ifdef DEBUG
    uart_string("Communication restored......!\r\n");
#endif
    NumISLDevices=NumDevices();
    clear_all_fault();clear_all_fault();//try to clear all faults
    InitializeISLParameters(NumISLDevices);
#ifdef DEBUG
    Uint8 _buf[8]={};
    uart_string("There are ");
    my_itoa(NumISLDevices, _buf);
    uart_string(_buf);
    uart_string(" devices!\r\n");
#endif
    fault_isr();
    ISL_ResetFlags();
    GetISLData(NumISLDevices);
    //DELAY_S(1);
    ISL_EnableReceiveCallback();
    ISL_SetReceiveCallback(RecieveHandler);                 // Set the call back to the recieve handler
    FaultLEDOff();
    //System restored, now try sending can messages again

}
