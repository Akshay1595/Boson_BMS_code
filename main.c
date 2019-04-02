//###########################################################################
//
// FILE:   can_external_transmit.c
//
// TITLE:  Example to demonstrate CAN external transmission
//
//! \addtogroup cpu01_example_list
//! <h1>CAN-A to CAN-B External Transmit (can_external_transmit)</h1>
//!
//! This example initializes CAN module A and CAN module B for external
//! communication. CAN-A module is setup to transmit incrementing data for "n"
//! number of times to the CAN-B module, where "n" is the value of TXCOUNT.
//! CAN-B module is setup to trigger an interrupt service routine (ISR) when
//! data is received. An error flag will be set if the transmitted data doesn't
//! match the received data.
//!
//! \note Both CAN modules on the device need to be
//!       connected to each other via CAN transceivers.
//!
//! \b Hardware \b Required \n
//!  - A C2000 board with two CAN transceivers
//!
//! \b External \b Connections \n
//!  - ControlCARD CANA is on GPIO31 (CANTXA) and GPIO30 (CANRXA)
//!  - ControlCARD CANB is on GPIO8 (CANTXB) and GPIO10 (CANRXB)
//!
//! \b Watch \b Variables \n
//!  - TXCOUNT - Adjust to set the number of messages to be transmitted
//!  - txMsgCount - A counter for the number of messages sent
//!  - rxMsgCount - A counter for the number of messages received
//!  - txMsgData - An array with the data being sent
//!  - rxMsgData - An array with the data that was received
//!  - errorFlag - A flag that indicates an error has occurred
//!
//
//###########################################################################
// $TI Release: F2837xD Support Library v3.05.00.00 $
// $Release Date: Thu Oct 18 15:48:42 CDT 2018 $
// $Copyright:
// Copyright (C) 2013-2018 Texas Instruments Incorporated - http://www.ti.com/
//
// Redistribution and use in source and binary forms, with or without 
// modification, are permitted provided that the following conditions 
// are met:
// 
//   Redistributions of source code must retain the above copyright 
//   notice, this list of conditions and the following disclaimer.
// 
//   Redistributions in binary form must reproduce the above copyright
//   notice, this list of conditions and the following disclaimer in the 
//   documentation and/or other materials provided with the   
//   distribution.
// 
//   Neither the name of Texas Instruments Incorporated nor the names of
//   its contributors may be used to endorse or promote products derived
//   from this software without specific prior written permission.
// 
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS 
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT 
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT 
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, 
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT 
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT 
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// $
//###########################################################################

//
// Included Files
//
#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "inc/hw_can.h"
#include "driverlib/can.h"
#include "my_can.h"
#include "General.h"
#include "ISL94212.h"
#include "Timer.h"
#include "SPI.h"
#include "device_implementation.h"
#include "uart.h"

//
// Globals
//
Uint8 NumISLDevices=0x00;


//
// Main
//
void main(void)
{

    DisableISR();

    Setup();

    uart_string_newline("Setup Complete!");
    TMR_Init();
    uart_string_newline("Timer Setup Complete!");
    SPI_Init();
    uart_string_newline("SPI Setup Complete!");
    can_init();
    uart_string_newline("CAN Setup Complete!");
    //SPI_Test();

    ResetISR();

    Bool Did_it_blend;

    Did_it_blend = ISL_Init_Retry(2);

    if (Did_it_blend == True)
        uart_string_newline("Yes it has connected Successfully!");
    else
    {
        uart_string_newline("I couldn't find ISL devices!");
        while(1);
    }

    NumISLDevices=NumDevices();
    ISL_EnableReceiveCallback();                                            // Enable the recieve call back
    ISL_SetReceiveCallback(RecieveHandler);                                 // Set the recievehandler to call when data is recieved from the daisy chain

    Uint8 _buf[8] = {};

    uart_string("There are ");
    my_itoa(NumISLDevices, _buf);
    uart_string(_buf);
    uart_string(" devices!\r\n");

    InitializeISLParameters(NumISLDevices);                                 // Initialize the default values into the ISL Registers

    DELAY_S(1);

    while(1) {
             GetISLData(NumISLDevices);
             DELAY_S(1);
             log_data();
    }
    asm("   ESTOP0");
}

//
// End of File
//
