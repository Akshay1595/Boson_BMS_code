/*
 *
 * General.h
 *
 * Author: Jama Mohamed
 *
 * Summary: General purpose and global routines
 *
 * Modified:
 *
 * 			 Added changelist   7/1/2014 JAM
 * 			 Addded ContinuousScanAll funciton
 *
 */

#ifndef GENERAL_H_
#define GENERAL_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File

#define GNR_CYCLES_PER_US 58
#define GNR_SPI_TIMER 0
#define GNR_ISL_TIMER 1

typedef enum {
	True,
	False
} Bool;

typedef enum {
	BLUE = 0x22,
	GREEN = 0x03,
	RED = 0x0A
}LED;

struct balance {
	Bool Enable;
	Uint16 BottomBalanceVoltage;
	Uint16 DeltaBalanceVoltage;
	Uint16 TopBalanceVoltage;
	Uint16 BalanceHysteresis;
	Uint16 BleedResistorDelay;
};

struct initialization {
	Uint16 MoreStuff;
};

struct limits {
	Uint16 TwentyFourVPower;
	Uint16 Overtemp;
	Uint16 Overvoltage;
	Uint16 Undervoltage;

};

struct thermal {
	Uint8 DeltaShelf;
	Uint8 DeltaModule;
	Uint8 MaxChip;
	Uint8 MaxCell;
	Uint8 MinCell;
	Uint8 LoopGain;
	Uint8 MinTurnOn;

};

typedef struct {     // bits description
   struct balance Balance;
   struct initialization Initialization;
   struct limits Limits;
   struct thermal Thermal;
} Parameters;

typedef struct {
	Bool OverTemp;
	Bool UnderTemp;
	Bool OverVoltage;
	Bool UnderVoltage;
	Bool TempSensor;
	Bool OpenWire;

}SummaryFaults;


typedef struct{

	Bool Chip1;
	Bool Chip2;
	Bool Chip3;
	Bool Chip4;
	Bool Chip5;
	Bool Chip6;
	Bool Chip7;
	Bool Chip8;
	Bool Chip9;
	Bool Chip10;
}ShelfArray;

void DELAY_MS(Uint16 seconds);
void DELAY_S(Uint8 seconds);

void DisableISR();
void EnableISR();
void ResetISR();

__interrupt void ADC0_ISR(void);
Uint16 GetCellsInBalance(void);
void BangTemperatureControl(void);
Uint16 GetNowCurrent(void);
Uint16 GetNowVoltage(void);
void ToggleLED(Uint8 led);
void BalanceCells(Uint16 CurrentDevice);
void InitializeISLParameters(Uint8 NumDevices);
void GetISLData(Uint8 NumDevices);
void TemperatureControl(void);
Uint8 GetMin(Uint16* Array, Uint8 Length);
Uint8 GetMax(Uint16* Array, Uint8 Length);
Uint16 GetAvg(Uint16* Array, Uint8 Length);
void InitializeAllParameters(void);
Parameters* GetParameters(void);
Bool MakeABool(Uint8 ToBeMade);
SummaryFaults* CheckFaults(Uint8 device);
void Setup();
void SetGPIO(Uint16 GPIOPin);
void ClearGPIO(Uint16 GPIOPin);
void LEDOn(LED Color);
void LEDOff(LED Color);




#endif /* GENERAL_H_ */
