/*
 * fault_handling.h
 *
 *  Created on: Apr 7, 2019
 *      Author: akshayg
 */

#ifndef INCLUDES_FAULT_HANDLING_H_
#define INCLUDES_FAULT_HANDLING_H_

#define FAULT_LED 32

//fault coding
#define Fault_UV            0x00
#define Fault_OV            0x01
#define Fault_OpenWire      0x02
#define Fault_OverTemp      0x03
#define Fault_OverCurrent   0x04
#define Fault_UnderTemp     0x05
#define Fault_Low_SOC       0x06

void fault_isr(void);
void FaultLEDOn(void);
void FaultLEDOff(void);
Bool checkForFault(void);
void alert_ecu(void);
void ConfigureFaultSetup(void);
void clear_all_fault(void);
void handle_fault(void);
SummaryFaults* CheckFaults(Uint8 device);
void recover_from_faults(void);
extern Uint16 NowCurrent;
extern Uint16 OverCurrentThreshold;

#endif /* INCLUDES_FAULT_HANDLING_H_ */
