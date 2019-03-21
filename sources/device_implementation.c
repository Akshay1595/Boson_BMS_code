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

extern Uint8 NumISLDevices;

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

float get_float_value_for_voltage(Uint16 voltage)
{
    float float_v = (float)0;
        if(voltage >= 8192)//negative voltage value
            float_v = ((float)((voltage - 16384) * 5)) /(float) 8192;
        else
            float_v = (float)voltage /(float) 8192;
    return float_v;
}
