//###########################################################################
//
// FILE:   main.c
//
// TITLE:  MAIN.c  file for BMS application
//
//###########################################################################
/*
 * main.c
 *
 *  Created on: Mar 8, 2019
 *      Author: Akshay Godase
 *
 *      Description:
 *      file for BMS which initializes all the accessories
 *      attached to BMS, read data continuously and check for fault
 */
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
    while(1) {
             COMMLEDToggle();
             GetISLData(NumISLDevices);
             DELAY_MS(INTERVAL);
#ifndef PARTIAL_LOG
             log_data();
#else
             partial_log();
#endif
             fault_isr();
    }
}
//
// End of File
//
