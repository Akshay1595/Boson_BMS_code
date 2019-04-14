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

tCANMsgObject INFO_MIN_MAX;
tCANMsgObject ALL_IN_ONE;

Uint8 DATA_MIN_MAX[8]={};
Uint8 DATA_ALL_IN_ONE[8]={};

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

#ifdef DEBUG
    uart_string("CAN Setup Complete!\r\n");
#endif

}

/*
 * This function initialize all the mailboxes needed for CAN data transmission
 */
void can_initialize_mailboxes()
{
    INFO_MIN_MAX.ui32MsgIDMask = 0;
    INFO_MIN_MAX.ui32Flags |= MSG_OBJ_EXTENDED_ID;
    INFO_MIN_MAX.ui32MsgLen = MSG_DATA_LENGTH;
    INFO_MIN_MAX.ui32MsgID = 0x0867;
    INFO_MIN_MAX.pucMsgData = DATA_MIN_MAX;

    ALL_IN_ONE.ui32MsgIDMask = 0;
    ALL_IN_ONE.ui32Flags |= MSG_OBJ_EXTENDED_ID;
    ALL_IN_ONE.ui32MsgID = 0x0868;
    ALL_IN_ONE.ui32MsgLen = MSG_DATA_LENGTH;
    ALL_IN_ONE.pucMsgData = DATA_ALL_IN_ONE;
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
        if(CurrentDevice == NumISLDevices)
        PackAndSendCellDetails();                                                // Pack the updated data and adjust for zero index
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
void PackAndSendCellDetails()
{
    Uint16 voltage_array[MAX_CELL_NUMBER*MAX_DEVICES],temp_array[MAX_DEVICES*4],Vbat=0;
    Uint16 Vcmax,Vcmin,Tcmax,Tcmin,Current,AmbientTemp;
    Uint8 i,cell_no,Soc;
    ISL_DEVICE *ISL_Struct;
    for(i=0;i<NumISLDevices;i++)
    {
        ISL_Struct = GetISLDevices(i);
        for(cell_no=1;cell_no<=MAX_CELL_NUMBER;cell_no++)
            voltage_array[((i*12)+(cell_no-1))] = read_voltage(i, cell_no);

        temp_array[i*4]     = ISL_Struct->PAGE1.TEMP.ET1V;
        temp_array[i*4+1]   = ISL_Struct->PAGE1.TEMP.ET2V;
        temp_array[i*4+2]   = ISL_Struct->PAGE1.TEMP.ET3V;
        temp_array[i*4+3]   = ISL_Struct->PAGE1.TEMP.ET4V;

        Vbat += ISL_Struct->PAGE1.CELLV.VB;
    }

    Vcmax = (0x3FFF) & (voltage_array[GetMax(voltage_array, (MAX_CELL_NUMBER*NumISLDevices))]);
    Vcmin = (0x3FFF) & (voltage_array[GetMin(voltage_array, (MAX_CELL_NUMBER*NumISLDevices))]);
    Tcmax = (0x3FFF) & (temp_array[GetMin(temp_array, 4*MAX_DEVICES)]);
    Tcmin = (0x3FFF) & (temp_array[GetMax(temp_array, 4*MAX_DEVICES)]);

    Soc = (Uint8)get_current_soc();
    Current = GetNowCurrent();
    AmbientTemp = 0x00;

    DATA_ALL_IN_ONE[0] = Soc;
    DATA_ALL_IN_ONE[1] = 0xFF & Current;            //lower byte
    DATA_ALL_IN_ONE[2] = 0xFF & (Current >>8);      //higher byte
    DATA_ALL_IN_ONE[3] = Vbat & 0xFF;               //lower byte
    DATA_ALL_IN_ONE[4] = 0xFF & (Vbat >> 8);        //higher byte
    DATA_ALL_IN_ONE[5] = 0xFF & AmbientTemp;
    DATA_ALL_IN_ONE[6] = 0xFF & (AmbientTemp>>8);
    DATA_ALL_IN_ONE[7] = 0x00;                      //reserved


    DATA_MIN_MAX[0] = 0xFF & (Vcmax);
    DATA_MIN_MAX[1] = 0xFF & (Vcmax>>8);
    DATA_MIN_MAX[2] = 0xFF & (Vcmin);
    DATA_MIN_MAX[3] = 0xFF & (Vcmin >> 8);
    DATA_MIN_MAX[4] = 0xFF & (Tcmax);
    DATA_MIN_MAX[5] = 0xFF & (Tcmax >> 8);
    DATA_MIN_MAX[6] = 0xFF & (Tcmin);
    DATA_MIN_MAX[7] = 0xFF & (Tcmin>>8);


    can_load_mailbox(&INFO_MIN_MAX);
    DELAY_US(800);
    can_load_mailbox(&ALL_IN_ONE);
    DELAY_US(800);
}
