/*
 * fault_handling.h
 *
 *  Created on: Apr 7, 2019
 *      Author: akshayg
 */

#ifndef INCLUDES_FAULT_HANDLING_H_
#define INCLUDES_FAULT_HANDLING_H_

#define FAULT_LED 2

void fault_isr(void);
void FaultLEDOn(void);
void FaultLEDOff(void);
Bool checkForFault(void);
void alert_ecu(Uint8 device);
void clear_all_fault(void);
void handle_fault(void);
SummaryFaults* CheckFaults(Uint8 device);
void recover_from_faults(void);
extern Uint16 NowCurrent;
extern Uint16 OverCurrentThreshold;

#endif /* INCLUDES_FAULT_HANDLING_H_ */
