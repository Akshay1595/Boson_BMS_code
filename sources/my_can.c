/*
 * my_can.c
 *
 *  Created on: Mar 8, 2019
 *      Author: Akshay Godase
 *
 *      Description:
 *      This CAN source file is about loading and sending CAN_mailboxes over a CAN bus
 *      THis file has all the functions necessary to do that
 *
 */

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "my_can.h"
#include "ISL94212.h"


// Globals
ISL_FLAGS* RecieveFlags;
ISL_DEVICE* ISL_Struct;
Uint8 CurrentDevice;
Uint32 SendOut[2];
Uint16 TemperatureArray[5];
Uint16 VoltageArray[18];
Uint16 ShelfMinVoltage[5];
Uint8 ShelfMinVoltageIndex[5];
Uint16 ShelfMaxVoltage[5];
Uint8 ShelfMaxVoltageIndex[5];
Uint16 ShelfAvgVoltage[5];
Uint16 ShelfMinTemperature[5];
Uint8 ShelfMinTemperatureIndex[5];
Uint16 ShelfMaxTemperature[5];
Uint8 ShelfMaxTemperatureIndex[5];
Uint16 ShelfAvgTemperature[5];
Uint16 ShelfVoltage;
Uint16 MinShelfVoltage=0xFFFF;
Uint16 MaxShelfVoltage=0x0000;
Bool OverTemp=False;
Bool UnderTemp=False;
Bool OverVoltage=False;
Bool UnderVoltage=False;
Bool TempSensor=False;
Bool OpenWire=False;
Uint8 FailCounter=0x00;
Uint8 CellsBalancing=0;
Uint8 Error=0;



/* can_init_GPIO:
 * This function configure GPIOS for CANBTXA and CANBRXA
 */
void can_init_GPIO()
{
    GPIO_SetupPinMux(CANRX_PIN, GPIO_MUX_CPU1, PIN_PERIPHERAL); //GPIO12 -  CANRXA
    GPIO_SetupPinOptions(CANRX_PIN, GPIO_INPUT, GPIO_ASYNC);
    GPIO_SetupPinMux(CANTX_PIN, GPIO_MUX_CPU1, PIN_PERIPHERAL); //GPIO17 - CANTXA
    GPIO_SetupPinOptions(CANTX_PIN, GPIO_OUTPUT, GPIO_PUSHPULL);
}

/* can_init() :
 * This function makes necessary initialization for CAN
 */

void can_init()
{
    can_init_GPIO();    // configure GPIOs first
    CANInit(CANTX_BASE);// init can for Transmission

    //
    // Setup CAN to be clocked off the PLL output clock
    //
    CANClkSourceSelect(CANTX_BASE, 0);   // 500kHz CAN-Clock

    // Set up the CAN bus bit rate to 500kHz for each module
    // This function sets up the CAN bus timing for a nominal configuration.
    // You can achieve more control over the CAN bus timing by using the
    // function CANBitTimingSet() instead of this one, if needed.
    // Additionally, consult the device data sheet for more information about
    // the CAN module clocking.
    //
    CANBitRateSet(CANTX_BASE, 200000000, 500000);
}

void can_load_mailbox(tCANMsgObject* load_mailbox)
{
    //
    // Load Transmit Message
    //
    CANMessageSet(CANTX_BASE, TX_MSG_OBJ_ID, load_mailbox,
                  MSG_OBJ_TYPE_TX);

}

void can_send()
{
    //
    // Start CAN module A and B operations
    //
    CANEnable(CANTX_BASE);
}

// The RecieveHandler code runs every time data is recieved from the ISL chips this triggers everything else to work
#pragma CODE_SECTION(RecieveHandler,".bigCode")
void RecieveHandler() {
    Uint8 Header;
    RecieveFlags=GetISLFlags();                                                                 // Pass the Flags
    CurrentDevice=RecieveFlags->Device;                                                         // Get the current device
    Header=RecieveFlags->Header;
    if(RecieveFlags->newData==True && RecieveFlags->newAck == False){                           // Only Pack Cell Information if there is new data
        //ISL_Struct=GetISLDevices(CurrentDevice);                                              // Get the struct for the current device
        if(Header>>4==READ_VOLTAGES>>4){
        //BalanceCells(CurrentDevice-1);                                                          // If we recieve a balance status message do the balancing sequence
        //PackAndSendCellDetails(CurrentDevice-1);                                                // Pack the updated data and adjust for zero index
        //PackAndSendModuleSummary(CurrentDevice-1);                                              // Pack and send the module summary
        //PackAndSendShelfSummary(CurrentDevice-1);                                               // Pack and send the shelf summary
        //PackAndSendShelfElectrical();                                                           // Pack and Send the shelf electrical
        }
        if(Header==BALANCE_STATUS){
        }
    }
    if(Header==NAK || Header==COMMS_FAILURE){
        FailCounter++;                                                                          // If we got a NAK then lets increase the fail counter
    }
    else{
        FailCounter=0;                                                                          // We only care about sequential NAK's so reset if it was just a one off
    }
    ISL_ResetFlags();                                                                           // Clear all of the flags
}
