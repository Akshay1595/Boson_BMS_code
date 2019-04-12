/*
 * my_can.h
 *
 *  Created on: Mar 12, 2019
 *      Author: Akshay Godase
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

extern Uint8 NumISLDevices;

//
//  function declarations
//
void can_init_GPIO(void);
void can_init(void);
void can_load_mailbox(tCANMsgObject*);
void RecieveHandler(void);
void can_receive_mailbox(tCANMsgObject* load_mailbox);
void PackAndSendCellDetails();
void can_initialize_mailboxes(void);
__interrupt void canbISR(void);

#endif /* INCLUDES_MY_CAN_H_ */
