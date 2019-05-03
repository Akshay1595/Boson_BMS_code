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
    Uint8 buf[16];
    double Vbat;
    float soc;
    Setup();
    while(1) {
             COMMLEDToggle();
             GetISLData(NumISLDevices);
             DELAY_MS(INTERVAL);
             Vbat = get_battery_voltage();float_to_ascii(Vbat, buf);uart_string(buf);uart_xmit(',');
             soc = get_current_soc();float_to_ascii(soc, buf);uart_string(buf);uart_string("\r\n");
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
