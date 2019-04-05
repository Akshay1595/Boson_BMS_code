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
#include "all_header.h"
//
// Main
//
void main(void)
{
    Setup();
    contactor_on();
    DELAY_S(1);

    while(1) {
             GetISLData(NumISLDevices);
             DELAY_S(1);
             log_data();
    }
}

//
// End of File
//
