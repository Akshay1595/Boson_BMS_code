/*
 * device_implementation.c
 *
 *  Created on: Mar 21, 2019
 *      Author: Akshay Godase
 *      Description: definition of all the functions declared in
 *      device_implementation.h
 */

#include "F28x_project.h"
#include "device_implementation.h"
#include "ISL94212.h"
#include "General.h"
#include "uart.h"
#include <math.h>

extern Uint8 NumISLDevices;

double ConvertTemperature(Uint16 Raw){
    double Vin_by_Vo = (double) 0x3FFF/(double)Raw;                                                               // Tempreg pulse is of 2.5V
    double r1 = 40.2 * 1000;
    double Rt = r1 / (Vin_by_Vo - 1);
    double logRt = log(Rt);
    double c1 = 1.009249522e-03, c2 = 2.378405444e-04, c3 = 2.019202697e-07;
    double Tc,T = (1.0 / (c1 + c2*logRt + c3*logRt*logRt*logRt));
    Tc = T - 273.15;
    return Tc;                                                                      // Use the Lookup table
}

double read_temp(Uint8 device,Uint8 temp_sensor_no)
{
    if ((device > NumISLDevices) || (temp_sensor_no > 4))               // there are only 4 temp sensors available
    {
        //error condition return doing nothing
        return 0;
    }
    double f_temp;
    Uint16 temp_V;
    ISL_DEVICE* ISLData;
    int16 change_from_normal_value;
    ISLData = GetISLDevices(device);
    switch(temp_sensor_no)
    {
        case 0:
            temp_V = (*ISLData).PAGE1.TEMP.ICT;
            change_from_normal_value =  temp_V  - 9180;                   //9180 is value at 25 degree celcius
            if(change_from_normal_value <0)
                f_temp = 25.00 - ( abs(change_from_normal_value)/(141.23)); // dividing it by 141.23 since 9180/65  65? range is from -40 to 85 DegreeC
            else
                f_temp = 25.00 + (abs(change_from_normal_value)/(141.23));
            return f_temp;

        case 1:
            temp_V = (*ISLData).PAGE1.TEMP.ET1V;
            f_temp = ConvertTemperature(temp_V);
            return f_temp;

        case 2:
            temp_V = (*ISLData).PAGE1.TEMP.ET2V;
            f_temp = ConvertTemperature(temp_V);
            return f_temp;

        case 3:
            temp_V = (*ISLData).PAGE1.TEMP.ET3V;
            f_temp = ConvertTemperature(temp_V);
            return f_temp;

        case 4:
            temp_V = (*ISLData).PAGE1.TEMP.ET4V;
            f_temp = ConvertTemperature(temp_V);
            return f_temp;

    }

        /*
    uart_string_newline("Temp Value I am seeing is: ");
    Uint8 buffer[20];
    float_to_ascii(temp_value, buffer);
    uart_string_newline(buffer);
    */
    return f_temp;
}

Uint16 read_voltage(Uint8 device,Uint8 cell_no)
{
    if ((device > NumISLDevices) || (cell_no > MAX_CELL_NUMBER))
    {
        //error condition return doing nothing
        return 0;
    }
    ISL_DEVICE* ISL_Data = GetISLDevices(device);

    switch(cell_no)
    {

        case VB:
            return ISL_Data->PAGE1.CELLV.VB;

        case C1V:
            return ISL_Data->PAGE1.CELLV.C1V;

        case C2V:
            return ISL_Data->PAGE1.CELLV.C2V;

        case C3V:
            return ISL_Data->PAGE1.CELLV.C3V;

        case C4V:
            return ISL_Data->PAGE1.CELLV.C4V;

        case C5V:
            return ISL_Data->PAGE1.CELLV.C5V;

        case C6V:
            return ISL_Data->PAGE1.CELLV.C6V;

        case C7V:
            return ISL_Data->PAGE1.CELLV.C7V;

        case C8V:
            return ISL_Data->PAGE1.CELLV.C8V;

        case C9V:
            return ISL_Data->PAGE1.CELLV.C9V;

        case C10V:
            return ISL_Data->PAGE1.CELLV.C10V;

        case C11V:
            return ISL_Data->PAGE1.CELLV.C11V;

        case C12V:
            return ISL_Data->PAGE1.CELLV.C12V;

        default:
            //error
            return 0;
    }
}
Uint16 read_current(void)
{
    return NowCurrent;
}
void balance_on(Uint8 device,Uint8 cell_no)
{
    if((device > NumISLDevices) && (cell_no > MAX_CELL_NUMBER))
    {
        //error
        return;
    }
    Uint8 BalanceChip[2];
    Uint16 Current_Balance_State = 0;
    ISL_DEVICE *ISLData;
    ISLData = GetISLDevices(device);
    Current_Balance_State =(*ISLData).PAGE2_2.SETUP.BSTAT.all;
    Current_Balance_State |= (1 << (cell_no-1));

    BalanceChip[0]=((Current_Balance_State>>8) & 0xFF);
    BalanceChip[1]=((Current_Balance_State) & 0x00FF);     //Bit Shift them to the right locations Chip 1 is 0-9
    //ISL_COMMAND deivce number is (2*Module)-1 for one and no minus 1 for the second
    ISL_Command(device+1,2,0x14,1,BalanceChip,2, 0);

}
void balance_off(Uint8 device,Uint8 cell_no)
{
    if((device > NumISLDevices) && (cell_no > MAX_CELL_NUMBER))
     {
         //error
         return;
     }
     Uint8 BalanceChip[2];
     Uint16 Current_Balance_State = 0;
     ISL_DEVICE *ISLData;
     ISLData = GetISLDevices(device);
     Current_Balance_State =(*ISLData).PAGE2_2.SETUP.BSTAT.all;
     Current_Balance_State &= ~(1 << (cell_no-1));

     BalanceChip[0]=((Current_Balance_State>>8) & 0xFF);
     BalanceChip[1]=((Current_Balance_State) & 0x00FF);     //Bit Shift them to the right locations Chip 1 is 0-9
     //ISL_COMMAND deivce number is (2*Module)-1 for one and no minus 1 for the second
     ISL_Command(device+1,2,0x14,1,BalanceChip,2, 0);

}
void balance_all(Uint8 device,Uint16 all_data)
{
    if((device > NumISLDevices) && (all_data >= (1<<13)) )
     {
         //error
         return;
     }

    Uint8 BalanceChip[2];
    Uint16 Current_Balance_State = 0;
    ISL_DEVICE *ISLData;
    ISLData = GetISLDevices(device);
    Current_Balance_State =(*ISLData).PAGE2_2.SETUP.BSTAT.all;
    Current_Balance_State = (all_data & 0x0FFF);

    BalanceChip[0]=((Current_Balance_State>>8) & 0xFF);
    BalanceChip[1]=((Current_Balance_State) & 0x00FF);     //Bit Shift them to the right locations Chip 1 is 0-9
    //ISL_COMMAND deivce number is (2*Module)-1 for one and no minus 1 for the second
    ISL_Command(device+1,2,0x14,1,BalanceChip,2, 0);
}
void contactor_on(void)
{
    //GPIO31 is connected to contactor
    GpioDataRegs.GPASET.bit.GPIO31 = 1;
}
void contactor_off(void)
{
    //GPIO31 is connected to contactor
    GpioDataRegs.GPACLEAR.bit.GPIO31 = 1;
}
Uint16 get_current_soc(void)
{
    //do nothing
    return 0;
}
void write_undervoltage_threshold(Uint8 device,float uv_voltage)
{
    Uint8 UnderVoltageLimit[2];
    UnderVoltageLimit[1]=(Uint16)(((1<<13)/5)*uv_voltage);                                            //Set Lower Limit to 1.05
    UnderVoltageLimit[0]=(Uint16)(((1<<13)/5)*uv_voltage)>>8;                                     //Set Lower Limit to 1.05

    ISL_WriteRegister(device,2,0x11,UnderVoltageLimit);                                          // Set UnderVoltage Limit
}
void write_overvoltage_threshold(Uint8 device,float ov_voltage)
{
    Uint8 OverVoltageLimit[2];

    OverVoltageLimit[1]=(Uint16)(((1<<13)/5)*ov_voltage);                                         //Set Upper Limit to 1.67
    OverVoltageLimit[0]=(Uint16)(((1<<13)/5)*ov_voltage)>>8;                                      //Set Upper Limit to 1.67
    ISL_WriteRegister(device,2,0x10,OverVoltageLimit);                                           // Set OverVoltage Limit
}

float get_float_value_for_voltage(Uint16 voltage,CELL_OR_PACK cell_or_pack)
{
    //refer to datasheet for formulae

    float float_v = (float)0;

    if (cell_or_pack == cell)
    {
        if(voltage >= 8192)//negative voltage value
            float_v = ((float)((voltage - 16384) * 5)) /(float) 8192;
        else
            float_v = ((float)voltage * 5) /(float) 8192;
    }
    else
    {
        if(voltage >= 8192)//negative voltage value
            float_v = ((float)((voltage - 16384) * 2.5 * (15.9350784))) /(float) 8192;
        else
            float_v = ((float)voltage * 2.5 * (15.9350784)) /(float) 8192;

    }
    return float_v;
}

void log_data()
{
    Uint8 NumOfISLDevices = NumDevices();
    Uint8 CurrentDevice;
    ISL_DEVICE* ISLData;
    uart_string_newline("----------------------------------------------Cell Data now----------------------------------------------");
    for(CurrentDevice=0 ; CurrentDevice < NumOfISLDevices; CurrentDevice++)
    {
        uart_string_newline("Device ");
        Uint8 buf[20]={};
        my_itoa(CurrentDevice, buf);
        uart_string(buf);

        uart_string_newline("Cell Voltages");
        uart_string_newline("Vb\tVc1\tVc2\tVc3\tVc4\tVc5\tVc6\tVc7\tVc8\tVc9\tVc10\tVc11\tVc12\r\n");

        Uint16 voltage;
        float f_voltage;
        Uint8 cell_no = 0;
        for(cell_no = 0; cell_no <13; cell_no++)
        {
            voltage = read_voltage(CurrentDevice, cell_no);
            if (cell_no == 0)
                f_voltage = get_float_value_for_voltage(voltage, pack);
            else
                f_voltage = get_float_value_for_voltage(voltage, cell);

            float_to_ascii(f_voltage, buf);
            uart_string(buf);
            uart_xmit('\t');
        }

        uart_string_newline("Cell Temperatures");
        uart_string_newline("Temp_IC\t\tTemp_T1\t\tTemp_t2\t\tTemp_t3\t\tTemp_t4\r\n");


        Uint8 temp_sensor_no = 0;
        double temp_degreeC;
        for(temp_sensor_no = 0; temp_sensor_no <5; temp_sensor_no++)
        {
            temp_degreeC = read_temp(CurrentDevice,temp_sensor_no);
            float_to_ascii(temp_degreeC, buf);
            uart_string(buf);
            uart_string("\t\t");
        }
        uart_string("\r\n");
    }
}
