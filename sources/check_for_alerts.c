/*
 * check_for_alerts.c
 *
 *  Created on: May 8, 2019
 *      Author: Akshay Godase
 *
 *   Description : This file checks if the data is reaching fault threshold, If yes, tries to intimate the VCU.
 */
//extern Uint8 NumISLDevices;
#include "all_header.h"

extern Uint8 NumISLDevices;

tCANMsgObject alertmailbox;

volatile alert_data alert_msgdata;

//
//This function initializes the CAN message required for a AlertMessages
//
#pragma CODE_SECTION(init_alert_task,".bigCode")
void init_alert_task(void)
{
    alertmailbox.pucMsgData = (void *)&alert_msgdata;
    alertmailbox.ui32MsgID = 0x300;
    alertmailbox.ui32MsgLen = 8;
    alertmailbox.ui32MsgIDMask = 0;
}

//
// This function checks if we reach the threshold or not?
//
#pragma CODE_SECTION(checkForAlerts,".bigCode")
void checkForAlerts(void)
{
    ISL_DEVICE *ISLData;
    float voltage_value;
    double temp_value;
    float SOC_value;
    Uint8 device = 0;
    Uint8 cell_no=0;
    Uint8 sens_no;
    for(device=0;device<NumISLDevices;device++)
    {
        //step1:
        ISLData = GetISLDevices(device);

        //step2:read Voltages and check
        for(cell_no=1;cell_no<=12;cell_no++)
        {
            if( (cell_no != 9) && (cell_no != 5) && (cell_no != 6) &&(cell_no!=7) && (cell_no != 8))
            {
                voltage_value = get_float_value_for_voltage(read_voltage(device, cell_no),cell);
                if(voltage_value < CELL_UV_ALERT)
                {
                    //reaching under voltage threshold be careful
                    inform_ecu(Fault_UV,device,cell_no);
                }
                else if(voltage_value > CELL_OV_ALERT)
                {
                    //reaching over voltage threshold be careful
                    inform_ecu(Fault_OV,device,cell_no);
                }
            }
        }

        //step3: read temps and check
        for(sens_no=1; sens_no <= 4; sens_no++)
        {
            temp_value = read_temp(device, sens_no);
            if (temp_value < TEMP_UNDER_ALERT)
            {
                //reaching under temperature threshold, be careful
                inform_ecu(Fault_UnderTemp,device,sens_no);
            }
            else if(temp_value < TEMP_UNDER_ALERT)
            {
                inform_ecu(Fault_OverTemp,device,sens_no);
            }
        }
    }
    //step4: read SOC and check
    SOC_value = get_current_soc();
    if(SOC_value <= SOC_LOW)
    {
        inform_ecu(Fault_Low_SOC, 0xFF, 0xFF);
    }
    else if(SOC_value >= SOC_FULL)
    {
        inform_ecu(Warning_Full_SOC, 0xFF, 0xFF);
    }

    //step5: read current and check
    if(GetNowCurrent()> CURRENT_ALERT)
    {
        inform_ecu(Fault_OverCurrent, 0xFF, 0xFF);
    }
}

//
//This function sends a CAN message with all the parameters
//
#pragma CODE_SECTION(inform_ecu,".bigCode")
void inform_ecu(Uint8 fault_code,Uint8 device,Uint8 cell_no)
{
    alert_msgdata.AlertCode = fault_code;
    alert_msgdata.device_no = device & 0x0F;
    alert_msgdata.cell_no = cell_no & 0x0F;
    alert_msgdata.resvd1= 0;
    alert_msgdata.resvd2 = 0;
    Uint16 value_now = getValueNow(fault_code, device, cell_no);
    alert_msgdata.NowValue_Low = value_now & 0x00FF;
    alert_msgdata.NowValue_High = (value_now >> 8) & 0x00FF;
    Uint16 threshold_value = get_threshold_value(fault_code);
    alert_msgdata.Threshold_Low = (threshold_value) & 0xFF00;
    alert_msgdata.Threshold_High = (threshold_value >> 8) & 0xFF00;
    can_load_mailbox(&alertmailbox);
}

//
//This function will give you current value for particular fault
//
#pragma CODE_SECTION(getValueNow,".bigCode")
Uint16 getValueNow(Uint8 fault_code,Uint8 device,Uint8 cell_no)
{
    Uint8 buff[64]={};
    Uint16 _16bit_Value;
    float soc_now,voltage;
    double temp;
#ifdef DEBUG
    uart_string("ALERT: ");
#endif
    ISL_DEVICE *ISL_Data = GetISLDevices(device);
    switch(fault_code)
    {
    //--------------------------------------------------------UV_ALERT-------------------------------------------------------------------//
        case Fault_UV:
            alert_msgdata.AlertCode = Fault_UV;
            _16bit_Value = read_voltage(device, cell_no);
            voltage = get_float_value_for_voltage(_16bit_Value,cell);
#ifdef DEBUG
            uart_string("UV Device:");
            my_itoa(device,buff);
            uart_string(buff);
            uart_string(" Cell_no: ");
            my_itoa(cell_no, buff);
            uart_string(buff);
            float_to_ascii(voltage, buff);
            uart_string(" Now Voltage = ");
            uart_string(buff);
            uart_string(" Threshold = ");
            float_to_ascii((get_float_value_for_voltage(get_threshold_value(fault_code),cell)),buff);
            uart_string(buff);uart_string("\r\n");
#endif
            return _16bit_Value;

        //--------------------------------------------------------OV_ALERT-------------------------------------------------------------------//
        case Fault_OV:
            _16bit_Value = read_voltage(device, cell_no);
            voltage = get_float_value_for_voltage(_16bit_Value,cell);
#ifdef DEBUG
            uart_string("OV Device:");
            my_itoa(device,buff);
            uart_string(buff);
            uart_string(" Cell_no: ");
            my_itoa(cell_no, buff);
            uart_string(buff);
            float_to_ascii(voltage, buff);
            uart_string(" Now Voltage = ");
            uart_string(buff);
            uart_string(" Threshold = ");
            float_to_ascii((get_float_value_for_voltage(get_threshold_value(fault_code),cell)),buff);
            uart_string(buff);uart_string("\r\n");
#endif
            return _16bit_Value;

        //----------------------------------------------------OverTemp_ALERT-------------------------------------------------------------------//
        case Fault_OverTemp:
            _16bit_Value = 0;
            switch(cell_no)
            {
                case 1:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET1V;
                    break;
                case 2:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET2V;
                    break;
                case 3:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET3V;
                    break;
                case 4:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET4V;
                    break;
                default:
                    break;
            }
#ifdef DEBUG
            uart_string("OverTemp Device:");
            my_itoa(device,buff);
            uart_string(buff);
            uart_string(" Sens_no: ");
            my_itoa(cell_no, buff);
            uart_string(buff);
            temp = read_temp(device, cell_no);
            float_to_ascii(temp, buff);
            uart_string(" Now Temp = ");
            uart_string(buff);
            uart_string(" Threshold = ");
            float_to_ascii((ConvertTemperature(get_threshold_value(fault_code))),buff);
            uart_string(buff);uart_string("\r\n");
#endif
            return _16bit_Value;


            //-------------------------------------------------OverCurrent_ALERT-------------------------------------------------------------------//
        case Fault_OverCurrent:
            _16bit_Value = GetNowCurrent();
            if (_16bit_Value > OverCurrentThreshold)
            {
#ifdef DEBUG
            uart_string("OverCurrent:");
            my_itoa(_16bit_Value,buff);
            uart_string(" Current Now: ");
            uart_string(buff);
            uart_string("\r\n");
#endif
            }
            return _16bit_Value;

        //----------------------------------------------UnderTemperature_ALERT-------------------------------------------------------------------//
        case Fault_UnderTemp:
            switch(cell_no)
            {
                case 1:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET1V;
                    break;
                case 2:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET2V;
                    break;
                case 3:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET3V;
                    break;
                case 4:
                    _16bit_Value = ISL_Data->PAGE1.TEMP.ET4V;
                    break;
                default:
                    _16bit_Value = 0;
                    break;
            }
#ifdef DEBUG
            uart_string("UnderTemp Device:");
            my_itoa(device,buff);
            uart_string(buff);
            uart_string(" Sens_no: ");
            my_itoa(cell_no, buff);
            uart_string(buff);
            temp = read_temp(device, cell_no);
            float_to_ascii(temp, buff);
            uart_string(" Now Temp = ");
            uart_string(buff);
            uart_string(" Threshold = ");
            float_to_ascii((ConvertTemperature(get_threshold_value(fault_code))),buff);
            uart_string(buff);uart_string("\r\n");
#endif
            return _16bit_Value;

        //-------------------------------------------------LOW_SOC_ALERT-------------------------------------------------------------------//
        case Fault_Low_SOC:
             soc_now = get_current_soc();
            _16bit_Value = (soc_now / 100) * (65535);
#ifdef DEBUG
             uart_string("Low_SOC");
             float_to_ascii(soc_now,buff);
             uart_string(" SOC_Now:");
             uart_string(buff);
             uart_string("\r\n");
#endif
            return _16bit_Value;

        //-------------------------------------------------FULL_SOC_ALERT-------------------------------------------------------------------//
        case Warning_Full_SOC:
            soc_now = get_current_soc();
            if (soc_now == 100.00)
            {
                _16bit_Value = 65535;                           //16 bit value for 100% SOC

#ifdef DEBUG
                uart_string("Full_SOC");
                float_to_ascii(soc_now,buff);
                uart_string(" SOC_Now:");
                uart_string(buff);
                uart_string("\r\n");
#endif
            }
            return _16bit_Value;

        default:
            _16bit_Value = 0;
            return _16bit_Value;
    }
}
