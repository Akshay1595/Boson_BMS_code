/*
 * device_implementation.h
 *
 *  Created on: Mar 21, 2019
 *      Author: Akshay Godase
 *      This file has all the important functions those will give you all the basic functionalities
 *      to interact with measurement device in application layer
 */

#ifndef DEVICE_IMPLEMENTATION_H_
#define DEVICE_IMPLEMENTATION_H_

#include "F28x_project.h"
#include "ISL94212.h"

#define MAX_CELL_NUMBER 12

typedef enum  {
    VB=0,
    C1V,
    C2V,
    C3V,
    C4V,
    C5V,
    C6V,
    C7V,
    C8V,
    C9V,
    C10V,
    C11V,
    C12V
}CELL_VOLTAGE_NUMBERING;

typedef enum{
    cell = 0,
    pack = 1
}CELL_OR_PACK;


extern Uint16 NowCurrent;

double ConvertTemperature(Uint16 Raw);
double read_temp(Uint8 device,Uint8 temp_sensor_no);
Uint16 read_voltage(Uint8 device,Uint8 cell_no);
float get_float_value_for_voltage(Uint16 voltage, CELL_OR_PACK cell_or_pack);
Uint16 read_current(void);
Uint16 get_current_soc(void);
void balance_on(Uint8 device,Uint8 cell_no);
void balance_off(Uint8 device,Uint8 cell_no);
void balance_all(Uint8 device,Uint16 all_data);
void contactor_on(void);
void contactor_off(void);
Uint16 get_current_soc(void);
double get_battery_voltage(void);
void log_data(void);
void set_over_temperature_limit(Uint8 device,double degreeC);
void write_undervoltage_threshold(Uint8 device,float uv_voltage);
void write_overvoltage_threshold(Uint8 device,float ov_voltage);
void disable_cell_from_faulting(Uint8 device,Uint8*array);
Uint16 get_current_soc(void);

#endif /* DEVICE_IMPLEMENTATION_H_ */
