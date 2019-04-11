/*
 * my_can.h
 *
 *  Created on: Mar 12, 2019
 *      Author: akshayg
 */

#ifndef INCLUDES_MY_CAN_H_
#define INCLUDES_MY_CAN_H_

#include "F28x_Project.h"
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_can.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/can.h"
#include "driverlib/debug.h"
#include "driverlib/interrupt.h"

typedef struct mailbox{
    tCANMsgObject CELLV_1_4;
    Uint8 cell_V_1_4[8];

    tCANMsgObject CELLV_5_8;
    Uint8 cell_V_5_8[8];

    tCANMsgObject CELLV_9_C;
    Uint8 cell_V_9_C[8];

    tCANMsgObject FAULT_DATA;
    Uint8 all_fault[8];

    tCANMsgObject TEMP_DATA;
    Uint8 all_temp[8];
}device_mailboxes;

//
// Defines
//
#define TXCOUNT  100
#define MSG_DATA_LENGTH    8
#define TX_MSG_OBJ_ID    1
#define RX_MSG_OBJ_ID    1
#define CANTX_PIN           12   //GPIO12
#define CANRX_PIN           17   //GPIO17
#define PIN_PERIPHERAL      2    //FOR GPIO12 AND GPIO17 PERIPHERAL IS 2 REFER TO DATASHEET
#define CAN_BASE          CANB_BASE   //CANB_BASE is configured for CAN data


/*
 * mailbox msgIDs
 * please refer https://docs.google.com/spreadsheets/d/1kbmyRu-nIQbgxeYA9p3WfBMIfylLFetZoJmql5Y0KBc/edit#gid=0
 */
#define MAX_TX_MAILBOXES 19     //below 19 mailboxes we have

#define CELLV_D1_C1_4   0x501114    // device 1 cellVoltage from cell1 to cell4
#define CELLV_D1_C5_8   0x501158
#define CELLV_D1_C9_C   0x50119C
#define CELLV_D2_C1_4   0x502114
#define CELLV_D2_C5_8   0x502158
#define CELLV_D2_C9_C   0x50219C
#define CELLV_D3_C1_4   0x503114
#define CELLV_D3_C5_8   0x503158
#define CELLV_D3_C9_C   0x50319C
#define CELLV_D4_C1_4   0x504114
#define CELLV_D4_C5_8   0x504158
#define CELLV_D4_C9_C   0x50419C
#define PACKV_D1_D4     0x514100    //pack voltages for Device1 to device4
#define IC_TEMP_D1_D4   0x514200    //IC_temperatures for device1 to device4
#define FAULT_D1        0x501300    //Fault data for device1
#define FAULT_D2        0x502300    //fault data for device2
#define FAULT_D3        0x503300
#define FAULT_D4        0x504300
#define SOC_CURRENT     0x514400    //SOC and current data

extern Uint8 NumISLDevices;

//
//  function declarations
//
void can_init_GPIO(void);
void can_init(void);
void can_load_mailbox(tCANMsgObject*);
void RecieveHandler(void);
void can_receive_mailbox(tCANMsgObject* load_mailbox);
void PackAndSendCellDetails(Uint8 device);
void can_initialize_mailboxes(void);
__interrupt void canbISR(void);

#endif /* INCLUDES_MY_CAN_H_ */
