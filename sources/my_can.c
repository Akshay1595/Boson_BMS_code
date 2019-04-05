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

#include "all_header.h"


// Globals
ISL_FLAGS* RecieveFlags;
ISL_DEVICE* ISL_Struct;
Uint8 CurrentDevice;
Uint8 FailCounter=0x00;
device_mailboxes  device_MBS[MAX_DEVICES];
tCANMsgObject PackVoltages;
Uint8 pack_voltages[8]={};                          // this array is for pack voltage variables
tCANMsgObject sRXCANMessage;                        // this variable is for data receive

/* can_init_GPIO:
 * This function configure GPIOS for CANBTX and CANBRX
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
    CANInit(CAN_BASE);// init can for Transmission

    //
    // Setup CAN to be clocked off the PLL output clock
    //
    CANClkSourceSelect(CAN_BASE, 0);   // 500kHz CAN-Clock

    // Set up the CAN bus bit rate to 500kHz for each module
    // This function sets up the CAN bus timing for a nominal configuration.
    // You can achieve more control over the CAN bus timing by using the
    // function CANBitTimingSet() instead of this one, if needed.
    // Additionally, consult the device data sheet for more information about
    // the CAN module clocking.
    //
    CANBitRateSet(CAN_BASE, 400000000, 500000);

    //
    // Enable interrupts on the CAN B peripheral.
    //
    CANIntEnable(CAN_BASE, CAN_INT_MASTER | CAN_INT_ERROR | CAN_INT_STATUS);

    DINT;

    EALLOW;
    PieVectTable.CANB0_INT = canbISR;
    EDIS;
    //
    // Enable the CAN-B interrupt on the processor (PIE).
    //
    PieCtrlRegs.PIEIER9.bit.INTx7 = 1;
    IER |= M_INT9;
    EINT;

    CANGlobalIntEnable(CAN_BASE, CAN_GLB_INT_CANINT0);

    //
    // Start CAN module B operations
    //
    CANEnable(CAN_BASE);
    can_initialize_mailboxes();
}

/*
 * This function initialize all the mailboxes needed for CAN data transmission
 */
void can_initialize_mailboxes()
{
    Uint8 device = 0;

    for(device=0;device<MAX_DEVICES;device++)
    {
        device_MBS[device].CELLV_1_4.ui32MsgID = 0x500114 | ((device+1)<<12);
        device_MBS[device].CELLV_1_4.pucMsgData = device_MBS[device].cell_V_1_4;

        device_MBS[device].CELLV_5_8.ui32MsgID = 0x500158 | ((device+1)<<12);
        device_MBS[device].CELLV_5_8.pucMsgData = device_MBS[device].cell_V_5_8;

        device_MBS[device].CELLV_9_C.ui32MsgID = 0x50019C | ((device+1)<<12);
        device_MBS[device].CELLV_9_C.pucMsgData = device_MBS[device].cell_V_9_C;

        device_MBS[device].FAULT_DATA.ui32MsgID = 0x500200 | ((device+1)<<12);
        device_MBS[device].FAULT_DATA.pucMsgData = device_MBS[device].all_fault;

        device_MBS[device].TEMP_DATA.ui32MsgID = 0x500300 | ((device+1)<<12);
        device_MBS[device].TEMP_DATA.pucMsgData = device_MBS[device].all_temp;
    }
    //initial packVoltage mailbox
    PackVoltages.ui32Flags = MSG_OBJ_EXTENDED_ID;
    PackVoltages.ui32MsgID = 0x514100;
    PackVoltages.pucMsgData = pack_voltages;
}

void can_load_mailbox(tCANMsgObject* load_mailbox)
{
    //
    // Load Transmit Message
    //
    load_mailbox->ui32Flags |= MSG_OBJ_EXTENDED_ID;
    load_mailbox->ui32MsgLen = MSG_DATA_LENGTH;
    load_mailbox->ui32MsgIDMask = 0;
    CANMessageSet(CAN_BASE, TX_MSG_OBJ_ID, load_mailbox,
                  MSG_OBJ_TYPE_TX);

}

void can_receive_mailbox(tCANMsgObject* load_mailbox)
{
    load_mailbox->ui32Flags |= MSG_OBJ_EXTENDED_ID;
    load_mailbox->ui32MsgLen = MSG_DATA_LENGTH;
    load_mailbox->ui32MsgIDMask = 0;
    CANMessageSet(CAN_BASE, RX_MSG_OBJ_ID, load_mailbox,
                  MSG_OBJ_TYPE_RX);
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
        PackAndSendCellDetails(CurrentDevice-1);                                                // Pack the updated data and adjust for zero index
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

__interrupt void canbISR(void)
{
    uint32_t status;

    //
    // Read the CAN-B interrupt status to find the cause of the interrupt
    //
    status = CANIntStatus(CAN_BASE, CAN_INT_STS_CAUSE);

    //
    // If the cause is a controller status interrupt, then get the status
    //
    if(status == CAN_INT_INT0ID_STATUS)
    {
        //
        // Read the controller status.  This will return a field of status
        // error bits that can indicate various errors.  Error processing
        // is not done in this example for simplicity.  Refer to the
        // API documentation for details about the error status bits.
        // The act of reading this status will clear the interrupt.
        //
        status = CANStatusGet(CAN_BASE, CAN_STS_CONTROL);

        //
        // Check to see if an error occurred.
        //
        if(((status  & ~(CAN_ES_RXOK)) != 7) &&
           ((status  & ~(CAN_ES_RXOK)) != 0))
        {
            //
            // Set a flag to indicate some errors may have occurred.
            //
            //errorFlag = 1;
        }
    }
    //
    // Check if the cause is the CAN-B receive message object 1
    //
    else if(status == RX_MSG_OBJ_ID)
    {
        //
        // Get the received message
        //
        CANMessageGet(CAN_BASE, RX_MSG_OBJ_ID, &sRXCANMessage, true);

        //
        // Getting to this point means that the RX interrupt occurred on
        // message object 1, and the message RX is complete.  Clear the
        // message object interrupt.
        //
        CANIntClear(CAN_BASE, RX_MSG_OBJ_ID);

    }
    //
    // If something unexpected caused the interrupt, this would handle it.
    //
    else
    {
        //
        // Spurious interrupt handling can go here.
        //
    }

    //
    // Clear the global interrupt flag for the CAN interrupt line
    //
    CANGlobalIntClear(CAN_BASE, CAN_GLB_INT_CANINT0);

    //
    // Acknowledge this interrupt located in group 9
    //
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;
}

/*
 * Please find summary and CAN ID in document
 * https://docs.google.com/spreadsheets/d/1kbmyRu-nIQbgxeYA9p3WfBMIfylLFetZoJmql5Y0KBc/edit?usp=sharing
 */
void PackAndSendCellDetails(Uint8 device)
{
    Uint16 voltage;

    ISL_Struct = GetISLDevices(device);
    voltage = ISL_Struct->PAGE1.CELLV.C1V;
    device_MBS[device].cell_V_1_4[0] =  voltage & (0x00FF);
    device_MBS[device].cell_V_1_4[1] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C2V;
    device_MBS[device].cell_V_1_4[2] =  voltage & (0x00FF);
    device_MBS[device].cell_V_1_4[3] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C3V;
    device_MBS[device].cell_V_1_4[4] =  voltage & (0x00FF);
    device_MBS[device].cell_V_1_4[5] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C4V;
    device_MBS[device].cell_V_1_4[6] =  voltage & (0x00FF);
    device_MBS[device].cell_V_1_4[7] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);

    voltage = ISL_Struct->PAGE1.CELLV.C5V;
    device_MBS[device].cell_V_5_8[0] =  voltage & (0x00FF);
    device_MBS[device].cell_V_5_8[1] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C6V;
    device_MBS[device].cell_V_5_8[2] =  voltage & (0x00FF);
    device_MBS[device].cell_V_5_8[3] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C7V;
    device_MBS[device].cell_V_5_8[4] =  voltage & (0x00FF);
    device_MBS[device].cell_V_5_8[5] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C8V;
    device_MBS[device].cell_V_5_8[6] =  voltage & (0x00FF);
    device_MBS[device].cell_V_5_8[7] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);

    voltage = ISL_Struct->PAGE1.CELLV.C9V;
    device_MBS[device].cell_V_9_C[0] =  voltage & (0x00FF);
    device_MBS[device].cell_V_9_C[1] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C10V;
    device_MBS[device].cell_V_9_C[2] =  voltage & (0x00FF);
    device_MBS[device].cell_V_9_C[3] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C11V;
    device_MBS[device].cell_V_9_C[4] =  voltage & (0x00FF);
    device_MBS[device].cell_V_9_C[5] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
    voltage = ISL_Struct->PAGE1.CELLV.C12V;
    device_MBS[device].cell_V_9_C[6] =  voltage & (0x00FF);
    device_MBS[device].cell_V_9_C[7] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);

    //fault data packing
    Uint16 fault;
    fault = ISL_Struct->PAGE2_1.FAULT.UF.all;
    device_MBS[device].all_fault[0] =  fault & (0x00FF);
    device_MBS[device].all_fault[1] =  (( 0xFF00 & fault) >> 8 )&(0x00FF);
    fault = ISL_Struct->PAGE2_1.FAULT.OF.all;
    device_MBS[device].all_fault[2] =  fault & (0x00FF);
    device_MBS[device].all_fault[3] =  (( 0xFF00 & fault) >> 8 )&(0x00FF);
    fault = ISL_Struct->PAGE2_1.FAULT.OC.all;
    device_MBS[device].all_fault[4] =  fault & (0x00FF);
    device_MBS[device].all_fault[5] =  (( 0xFF00 & fault) >> 8 )&(0x00FF);
    fault = ISL_Struct->PAGE2_1.FAULT.OVTF.all;
    device_MBS[device].all_fault[6] =  fault & (0x00FF);
    device_MBS[device].all_fault[7] =  (( 0xFF00 & fault) >> 8 )&(0x00FF);


    //temperature data packing
    Uint16 temperature;
    temperature = ISL_Struct->PAGE1.TEMP.ET1V;
    device_MBS[device].all_temp[0] =  temperature & (0x00FF);
    device_MBS[device].all_temp[1] =  (( 0xFF00 & temperature) >> 8 )&(0x00FF);
    temperature = ISL_Struct->PAGE1.TEMP.ET2V;
    device_MBS[device].all_temp[2] =  temperature & (0x00FF);
    device_MBS[device].all_temp[3] =  (( 0xFF00 & temperature) >> 8 )&(0x00FF);
    temperature = ISL_Struct->PAGE1.TEMP.ET3V;
    device_MBS[device].all_temp[4] =  temperature & (0x00FF);
    device_MBS[device].all_temp[5] =  (( 0xFF00 & temperature) >> 8 )&(0x00FF);
    temperature = ISL_Struct->PAGE1.TEMP.ET4V;
    device_MBS[device].all_temp[6] =  temperature & (0x00FF);
    device_MBS[device].all_temp[7] =  (( 0xFF00 & temperature) >> 8 )&(0x00FF);

    can_load_mailbox(&device_MBS[device].CELLV_1_4);
    DELAY_US(800);           //note that delay is must if delay is not there transmission skips some of the ids
                            // found after some experiments
    can_load_mailbox(&device_MBS[device].CELLV_5_8);
    DELAY_US(800);
    can_load_mailbox(&device_MBS[device].CELLV_9_C);
    DELAY_US(800);
    can_load_mailbox(&device_MBS[device].FAULT_DATA);
    DELAY_US(800);
    can_load_mailbox(&device_MBS[device].TEMP_DATA);
    DELAY_US(800);

    if (device == (NumISLDevices-1))
    {
        Uint8 i = 0 ;
        for(;i < NumISLDevices; i++)
        {
            ISL_Struct = GetISLDevices(i);
            voltage = ISL_Struct->PAGE1.CELLV.VB;
            pack_voltages[i*2] =  voltage & (0x00FF);
            pack_voltages[i*2+1] =  (( 0xFF00 & voltage) >> 8 )&(0x00FF);
        }
        can_load_mailbox(&PackVoltages);
        DELAY_US(800);
    }
}
