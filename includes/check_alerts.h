/*
 * check_alerts.h
 *
 *  Created on: May 14, 2019
 *      Author: akshayg
 */

#ifndef INCLUDES_CHECK_ALERTS_H_
#define INCLUDES_CHECK_ALERTS_H_

#include "all_header.h"

#define CELL_UV_ALERT       2.8
#define CELL_OV_ALERT       4.1
#define CURRENT_ALERT       4090
#define TEMP_UNDER_ALERT    4.00
#define TEMP_OVER_ALERT     50.00
#define SOC_FULL            100.00
#define SOC_LOW             10.00

void inform_ecu(Uint8 fault_code,Uint8 device,Uint8 cell_no);
Uint16 getValueNow(Uint8 fault_code,Uint8 device,Uint8 cell_no);
void checkForAlerts(void);
void init_alert_task(void);

typedef struct ALERT_DATA{
    Uint8 AlertCode;
    Uint8 device_no:4;
    Uint8 cell_no:4;
    Uint8 NowValue_Low;
    Uint8 NowValue_High;
    Uint8 Threshold_Low;
    Uint8 Threshold_High;
    Uint8 resvd1;
    Uint8 resvd2;
}alert_data;



#endif /* INCLUDES_CHECK_ALERTS_H_ */
