/*
 * device_implementation.c
 *
 *  Created on: Mar 21, 2019
 *      Author: Akshay Godase
 *      Description: definition of all the functions declared in
 *      device_implementation.h
 */

#include "all_header.h"

//globals

//find look up table here
//https://docs.google.com/document/d/1JUxwLQbjrIoaSeL-24jZHb-qJ2MlkH5eazI8PYYRZg4/edit
Uint8 temp_lookup_table[8][2]=
{
 {0x0C,0x9F},//25degreeC
 {0x0A,0xC9},//30degreeC
 {0x07,0xDD},//35degreeC
 {0x07,0x90},//40degreeC
 {0x06,0xB8},//45degreeC
 {0x05,0xBE},//50degreeC
 {0x04,0xEA},//55degreeC
 {0x04,0x37},//60degreeC
};

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
// read the current from ADC
Uint16 read_current(void)
{
    return NowCurrent;
}

// battery voltage is some of all the pack voltage since they are in series
double get_battery_voltage(void)
{
    Uint16 batt_v = 0,i;
    double f_voltage = 0.0;
    ISL_DEVICE* ISL_Struct;
    for(i=0;i<NumISLDevices;i++)
    {
        ISL_Struct = GetISLDevices(i);
        batt_v =   ISL_Struct->PAGE1.CELLV.VB;
        f_voltage += get_float_value_for_voltage(batt_v, pack);
    }
    return f_voltage;
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
#ifdef DEBUG
    uart_string("Contactor On!\r\n");
#endif

    GPIO_WritePin(CONTACTOR_PIN, 1);
}
void contactor_off(void)
{
#ifdef DEBUG
    uart_string("Contactor off!\r\n");
#endif
    //GPIO31 is connected to contactor
    GPIO_WritePin(CONTACTOR_PIN, 0);

}
void contactor_gpio_setup()
{
    GPIO_SetupPinMux(CONTACTOR_PIN, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(CONTACTOR_PIN, GPIO_OUTPUT, GPIO_PUSHPULL);
}

//calculation of SOC based on lookup table
float get_current_soc(void)
{

    float soc_table[77][2] = {
        114.1764288, 99.99720885,
        114.1745829, 99.99696726,
        114.1703619, 99.99641468,
        114.1661426, 99.99586211,
        114.1619251, 99.99530957,
        114.1577092, 99.99475704,
        114.1534951, 99.99420454,
        114.1492827, 99.99365206,
        114.1388897, 99.99228807,
        114.128507,  99.99092421,
        114.1181349, 99.98956047,
        114.1077731, 99.98819686,
        114.0974217, 99.98683336,
        114.0870808, 99.98547,
        114.0651558, 99.98257526,
        114.0432776, 99.97968107,
        114.0214459, 99.97678744,
        113.9996608, 99.97389436,
        113.977922,  99.97100184,
        113.7982798, 99.94688216,
        113.6217959, 99.92280016,
        113.44841,   99.89875522,
        113.2780616, 99.87474668,
        112.4389919, 99.75046182,
        111.6727983, 99.62706108,
        110.9725149, 99.50446739,
        110.3319331, 99.38261133,
        109.7455048, 99.26143024,
        108.5900801, 98.98865642,
        107.6394421, 98.71850126,
        106.8547731, 98.45050626,
        106.2051081, 98.18429905,
        105.6655965, 97.91957489,
        105.216252,  97.65608373,
        104.5926971, 97.19122094,
        104.1355578, 96.72871771,
        103.7978565, 96.26795485,
        103.5461958, 95.80849389,
        103.3566186, 95.35001522,
        103.2120474,94.89228031,
        103.0443095, 94.1578326,
        102.9289502, 93.42435591,
        102.8460618, 92.69156569,
        102.7830361, 91.95930376,
        102.7319348, 91.22746788,
        102.6880141, 90.49598056,
        102.6096008, 89.00171248,
        102.5380211, 87.50850241,
        102.4681799, 86.01631805,
        102.3973228, 84.52517724,
        102.3243485, 83.03510275,
        102.2491024, 81.54610456,
        102.0982353, 78.7009122,
        101.9376967, 75.86002561,
        101.7658907, 73.02375214,
        101.5814426, 70.192462,
        101.3831786, 67.36651203,
        101.169613,  64.54627358,
        100.8001805, 60.14592184,
        100.3799965, 55.76271777,
        99.89819018, 51.39910049,
        99.34058937, 47.05805905,
        98.68837467, 42.74329427,
        97.91614907, 38.45945878,
        96.98889774, 34.21250613,
        95.85713077, 30.01021248,
        94.44891849, 25.86297484,
        92.65637549, 21.78507156,
        90.31193616, 17.79673209,
        87.14811891, 13.92807774,
        82.71204674, 10.2243277,
        76.23264744, 6.75669091,
        66.46582638, 3.640595218,
        62.62586369, 2.795770549,
        58.31494473, 2.004380491,
        53.52530405, 1.272808821,
        48.28517327, 0.6069323966
    };
    //do nothing
    int8 i = 76;
    float input_value = get_battery_voltage();

    //if voltage is more than max available in truth table then SOC will be obviously 100%
    if(input_value > soc_table[i][0])
        return 100.00;
    else if(input_value < soc_table[0][0])
        return 0.00;

    for(i=76;i>0;i--)
    {
        //printf("checking %f and %f with %f\n",soc_table[i][0],soc_table[i+1][0],input_value);
        if((input_value < soc_table[i][0]) && (input_value > soc_table[i+1][0]) )
        {
            //printf("%f is less than %f and greater than %f soc = %f\t",input_value,soc_table[i][0],soc_table[i+1][0],soc_table[i][1]);
            if((soc_table[i][0] - input_value) < (soc_table[i+1][0] - input_value) )
            {
                return soc_table[i][1];
                //printf("SOC = %f\n",soc_table[i][1]);
            }
            else
            {
                return soc_table[i+1][1];
                //printf("SOC = %f\n",soc_table[i+1][1]);
            }
            break;
        }
    }
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

// this function disable cells from faulting and measurements
// this function is called when you don't have 12 cells but you have some other number of cells
// Uint8 array is an array of 12 integers where you will pass the cells to be disabled from faulting
// ex. array[12]= {0,0,0,0,0,1,1,1,0,0,0,0}
// array above will disable cells 5,6,7

void disable_cell_from_faulting(Uint8 device,Uint8* array)
{
    Uint8 CellSetup[2]={};
    Uint8 i=0;
    for(i=0;i<8;i++)
    {
        if(array[i])
            CellSetup[1] |= (1<<i);
    }
    for(i=8;i<12;i++)
    {
        if(array[i])
            CellSetup[0] |= (1<<(i-8));
    }
    ISL_WriteRegister(device,2,0x05,CellSetup);
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
#ifdef DEBUG
#ifndef PARTIAL_LOG
    Uint8 NumOfISLDevices = NumDevices();
    Uint8 CurrentDevice;
    ISL_DEVICE* ISLData;
    uart_string("\r\n");
    uart_string("Device,");uart_string("Param,");uart_string("PackV,C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,BatteryVoltage\r\n");

    for(CurrentDevice=0 ; CurrentDevice < NumOfISLDevices; CurrentDevice++)
    {
        Uint8 buf[20]={};
        my_itoa(CurrentDevice, buf);
        uart_string(buf);uart_xmit(',');uart_string("Cell_Voltages,");
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
            uart_xmit(',');
        }
        f_voltage  = get_battery_voltage();
        float_to_ascii(f_voltage, buf);
        uart_string(buf);
        uart_xmit(',');

        uart_string("\r\n");
        my_itoa(CurrentDevice, buf);
        uart_string(buf);uart_xmit(',');uart_string("Cell_Temp,");

        Uint8 temp_sensor_no = 0;
        double temp_degreeC;
        for(temp_sensor_no = 0; temp_sensor_no <5; temp_sensor_no++)
        {
            temp_degreeC = read_temp(CurrentDevice,temp_sensor_no);
            float_to_ascii(temp_degreeC, buf);
            uart_string(buf);
            uart_xmit(',');
        }


        uart_string("\r\n");
        my_itoa(CurrentDevice, buf);
        uart_string(buf);uart_xmit(',');uart_string("UnderVoltage,");
        ISLData = GetISLDevices(CurrentDevice);
        Uint16 fault_data = (*ISLData).PAGE2_1.FAULT.UF.all;
        char i;
        for(i = 0;i<12;i++)
        {
            if(fault_data & (1<<i))
                uart_string("1,");
            else
                uart_string("0,");
        }

        uart_string("\r\n");
        my_itoa(CurrentDevice, buf);
        uart_string(buf);uart_xmit(',');uart_string("OverVoltage,");
        ISLData = GetISLDevices(CurrentDevice);
        fault_data = (*ISLData).PAGE2_1.FAULT.OF.all;
        for(i = 11;i>-1;i--)
        {
            if(fault_data & (1<<i))
                uart_string("1,");
            else
                uart_string("0,");
        }

        uart_string("\r\n");
        my_itoa(CurrentDevice, buf);
        uart_string(buf);uart_xmit(',');uart_string("OpenWire,");
        ISLData = GetISLDevices(CurrentDevice);
        fault_data = (*ISLData).PAGE2_1.FAULT.OC.all;
        for(i = 11;i>-1;i--)
        {
            if(fault_data & (1<<i))
                uart_string("1,");
            else
                uart_string("0,");
        }

        uart_string("\r\n");
        my_itoa(CurrentDevice, buf);
        uart_string(buf);uart_xmit(',');uart_string("OverTemp,");
        ISLData = GetISLDevices(CurrentDevice);
        fault_data = (*ISLData).PAGE2_1.FAULT.OVTF.all;
        for(i = 0;i<=4;i++)
        {
            uart_string("Temp_");
            my_itoa(i,buf);
            uart_string(buf);
            uart_xmit('=');
            if(fault_data & (1<<i))
                uart_string("1,");
            else
                uart_string("0,");
        }
        uart_string("\r\n");
    }
#endif
#endif
}
//we are referring a formula  Rt = R1 / ((Vo/Vin)-1) and we calculate Rt
//please find brief documentation at
//  https://docs.google.com/document/d/1JUxwLQbjrIoaSeL-24jZHb-qJ2MlkH5eazI8PYYRZg4/edit
void set_over_temperature_limit(Uint8 device,Uint8 degreeC)
{
    if(degreeC < 25 || degreeC >60)
    {
        //error since long wrong is passed
        return;
    }
    ISL_WriteRegister(device,2,0x12,temp_lookup_table[(degreeC/5)-5]);                                              // Set OverTemp to 55 Degrees C
}
//this is partial log which eliminates unnecessary information
//It is just giving info about min,max and faults
void partial_log()
{
#ifdef PARTIAL_LOG
    Uint16 V_array[7]={},Vcmin,Vcmax,i=0,Vpack;
    Uint16 Temp_array[4] = {};
    double F_value;
    Uint8 cell_no=1,index=0;
    Uint8 buf[16] = {};
    for(i=0;i<NumISLDevices;i++)
    {
        //get Vcmax
        index = 0;
        for(cell_no = 1;cell_no <= 12 ; cell_no++)
        {
            if( (cell_no != 9) && (cell_no != 5) && (cell_no != 6) &&(cell_no!=7) && (cell_no != 8))
            V_array[index++] = read_voltage(i, cell_no);
        }
        Vcmax=(Uint16)GetMax(V_array, 7);      //get the array index
        Vcmin=(Uint16)GetMin(V_array, 7);
        Vcmax = V_array[Vcmax];                 //get the value same variable for saving memory
        Vcmin = V_array[Vcmin];
        Vpack = read_voltage(i, 0);
        F_value = get_float_value_for_voltage(Vcmin, cell);float_to_ascii(F_value, buf);uart_string(buf);uart_xmit(',');
        F_value = get_float_value_for_voltage(Vcmax, cell);float_to_ascii(F_value, buf);uart_string(buf);uart_xmit(',');
        F_value = get_float_value_for_voltage(Vpack, pack);float_to_ascii(F_value, buf);uart_string(buf);uart_xmit(',');

        ISL_DEVICE* ISLData;
        ISLData = GetISLDevices(i);
        Temp_array[0] = ISLData->PAGE1.TEMP.ET1V;Temp_array[1] = ISLData->PAGE1.TEMP.ET2V;
        Temp_array[2] = ISLData->PAGE1.TEMP.ET3V;Temp_array[3] = ISLData->PAGE1.TEMP.ET4V;
        F_value = read_temp(i, (GetMax(Temp_array, 4)+1));float_to_ascii(F_value, buf);uart_string(buf);uart_xmit(',');
        F_value = read_temp(i, (GetMin(Temp_array, 4)+1));float_to_ascii(F_value, buf);uart_string(buf);uart_xmit(',');

        Uint16 somevalue;
        somevalue = ISLData->PAGE2_1.FAULT.OVTF.all;somevalue = (somevalue>>1) & 0x000F;my_itoa(somevalue, buf);uart_string(buf);uart_xmit(',');
        somevalue = ISLData->PAGE2_1.FAULT.UF.all;my_itoa(somevalue, buf);uart_string(buf);uart_xmit(',');
        somevalue = ISLData->PAGE2_1.FAULT.OF.all;my_itoa(somevalue, buf);uart_string(buf);uart_xmit(',');
        somevalue = ISLData->PAGE2_1.FAULT.OC.all;my_itoa(somevalue, buf);uart_string(buf);uart_xmit(',');

        if(i == NumISLDevices-1){
            float_to_ascii(get_ambient_temperature(), buf);
            uart_string(buf);
            uart_xmit(',');
            float_to_ascii(get_current_soc(), buf);
            uart_string(buf);
            uart_string("\r\n");
        }
        else
            uart_xmit(',');
    }
#endif
}

void COMMLEDSetup()
{
    EALLOW;
    GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;  // GPIO6 = GPIO6
    GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;   // GPIO6 = output
    GpioCtrlRegs.GPAPUD.bit.GPIO19 = 0;   // Enable pullup on GPIO6
    EDIS;
}
void COMLEDOn()
{
    GpioDataRegs.GPASET.bit.GPIO19 = 1;   // Load output latch
}
void COMLEDOff()
{
    GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;   // Load output latch
}
void COMMLEDToggle()
{
    GpioDataRegs.GPATOGGLE.bit.GPIO19 = 1;   // Load output latch
}
