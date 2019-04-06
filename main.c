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
