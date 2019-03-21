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
#define CANTX_BASE          CANB_BASE   //CANB_BASE is configured for TX

//
//  function declarations
//
void can_init_GPIO(void);
void can_init(void);
void can_load_mailbox(tCANMsgObject*);
void can_send(void);
void RecieveHandler(void);
//__interrupt void canbISR(void);  commented our currently since there is no data reception enabled

#endif /* INCLUDES_MY_CAN_H_ */
