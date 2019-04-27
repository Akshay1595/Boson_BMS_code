/*
 * adc.h
 *
 *  Created on: Mar 14, 2019
 *      Author: Akshay Godase
 *      Description:
 *      this file contains all teh declaration required for ADC file
 */

#ifndef INCLUDES_ADC_H_
#define INCLUDES_ADC_H_

#include "F28x_Project.h"
void ConfigureADC(void);
void ConfigureEPWM(void);
void SetupADCEpwm(Uint16 channel);
void setup_adc(void);
double _ConvertTemperature(Uint16 tempValue);
Uint16 Now_amb_temp(void);
double get_ambient_temperature(void);
double convertToCurrent(Uint16 value);
interrupt void adca1_isr(void);
interrupt void adca2_isr(void);
#endif /* INCLUDES_ADC_H_ */
