/*
 * fault_handling.c
 *
 *  Created on: Apr 7, 2019
 *      Author: Akshay Godase
 *      Description: This file contains all the handling and detection of faults
 *      If fault is detected, steps are like below
 *      1. Message to ECU
 *      2. Contactor off
 *      3. Wait till fault is recovered
 *      4. Once recovered turn on the contactor
 *      5. Message to ECU
 *      6. reset all ISL devices and reinitialize them
 *      7. read Isl data and start normal execution
 */

#include "all_header.h"

volatile SummaryFaults AllFaults[MAX_DEVICES];
volatile ISL_FLAGS *currentFlags;
tCANMsgObject FaultMessage;
Uint8 Fault_data[8] = {};
extern Uint16 OverCurrentThreshold;
extern Uint8 temp_lookup_table[8][2];


void fault_isr(void)
{
    Bool is_fault = False;
    is_fault = checkForFault();
    if(is_fault == True)
        handle_fault();
}

SummaryFaults* CheckFaults(Uint8 device){
    ISL_DEVICE* ISLData = GetISLDevices(device);                                                //Get the Current Module
    AllFaults[device].IsFault=False;AllFaults[device].OverCurrent=False;
    AllFaults[device].OpenWire=False;AllFaults[device].UnderVoltage = False;
    AllFaults[device].OverVoltage = False;AllFaults[device].OverTemp = False;AllFaults[device].UnderTemp = False;
    if((ISLData->PAGE2_1.FAULT.OC.all & ~(7<<5))>0){
        AllFaults[device].OpenWire=True;
        AllFaults[device].IsFault=True;
    }
    if((ISLData->PAGE2_1.FAULT.OF.all & ~(7<<5))>0){
        AllFaults[device].OverVoltage=True;
        AllFaults[device].IsFault=True;
    }
    if((ISLData->PAGE2_1.FAULT.UF.all & ~(7<<5))>0){
        AllFaults[device].UnderVoltage=True;
        AllFaults[device].IsFault=True;
    }
    if((ISLData->PAGE2_1.FAULT.OVTF.all)>0){
        AllFaults[device].OverTemp=True;
        AllFaults[device].IsFault=True;
    }
    if (NowCurrent > OverCurrentThreshold)
    {
        AllFaults[device].OverCurrent =True;
        AllFaults[device].IsFault=True;
    }
    return &AllFaults;
}
void FaultLEDOn()
{
    GPIO_WritePin(FAULT_LED, 1);
}

void FaultLEDOff()
{
    GPIO_WritePin(FAULT_LED, 0);
}

void ConfigureFaultSetup(void)
{
    //GPIO Fault LED Pin
    GPIO_SetupPinMux(FAULT_LED, GPIO_MUX_CPU1, 0);
    GPIO_SetupPinOptions(FAULT_LED, GPIO_OUTPUT, GPIO_PUSHPULL);
    FaultLEDOff();

    //setup CAN message
    FaultMessage.ui32MsgLen = 8;
    FaultMessage.ui32MsgIDMask = 0;
    FaultMessage.ui32MsgID = 0x75;
    FaultMessage.ui32Flags = 0;
    FaultMessage.pucMsgData = Fault_data;

}
Bool checkForFault(void)
{
    Bool is_fault = False;
    Uint8 CurrentDevice =0;
    for(;CurrentDevice<NumISLDevices;CurrentDevice++)
    {
        CheckFaults(CurrentDevice);
        if(AllFaults[CurrentDevice].IsFault == True)
            is_fault = True;
    }
    return is_fault;
}

void alert_ecu()
{
    ISL_DEVICE *ISL_Struct;
    Uint8 CurrentDevice = 0,cell_no=0,FaultCode=0,i;
    for(CurrentDevice=0;CurrentDevice<NumISLDevices;CurrentDevice++)
    {
        if (AllFaults[CurrentDevice].IsFault == True)
        {
            if(AllFaults[CurrentDevice].UnderVoltage == True){
                FaultCode = Fault_UV;ISL_Struct = GetISLDevices(CurrentDevice);
                for(i=0;i<12;i++){if(ISL_Struct->PAGE2_1.FAULT.UF.all & (1 << i)){cell_no = i+1;break;}}
            }
            if(AllFaults[CurrentDevice].OverVoltage == True){
                FaultCode = Fault_OV;ISL_Struct = GetISLDevices(CurrentDevice);
                for(i=0;i<12;i++){if(ISL_Struct->PAGE2_1.FAULT.OF.all & (1 << i)){cell_no = i+1;break;}}
            }
            if(AllFaults[CurrentDevice].OverTemp == True){
                FaultCode = Fault_OverTemp;ISL_Struct = GetISLDevices(CurrentDevice);
                for(i=0;i<5;i++){if(ISL_Struct->PAGE2_1.FAULT.OVTF.all & (1 << i)){cell_no = i+1;break;}}
            }
            if(AllFaults[CurrentDevice].OverCurrent == True){
                FaultCode = Fault_OverCurrent;cell_no = 0;
            }
            if(AllFaults[CurrentDevice].OpenWire == True){
                FaultCode = Fault_OpenWire;ISL_Struct = GetISLDevices(CurrentDevice);
                for(i=0;i<12;i++){if(ISL_Struct->PAGE2_1.FAULT.OC.all & (1 << i)){cell_no = i+1;break;}}
            }
            fill_can_message(FaultCode, CurrentDevice, cell_no);
            can_load_mailbox(&FaultMessage);
        }
    }

}

#pragma CODE_SECTION(handle_fault,".bigCode")
void handle_fault(void)
{

    FaultLEDOn();
    //disable receive callback so that this callback doesn't hinder normal fault handling
    ISL_DisableReceiveCallback();

    AGAIN:
    //step1: notify ECU
    alert_ecu();

    //step2: turn off the contactor
    contactor_off();

#ifdef DEBUG
    uart_string("Fault detected hence turning off the contactor first and waiting for fault recovery\r\n");
#endif

    //step3: wait for fault_recovery
    recover_from_faults();

#ifdef DEBUG
    uart_string("ReCovered! now read back data again and see if fault is there or not?\r\n");
#endif

    //step4:
    GetISLData(NumISLDevices);
    DELAY_MS(100);

    Bool is_fault;
    is_fault = checkForFault();

    if (is_fault == True)
    {
#ifdef DEBUG
        uart_string("Could not recover from fault hence jumping to handle fault routine again...!\r\n");
#endif
        goto AGAIN;
    }
    //step5 turn on contactor and return to normal execution
#ifdef DEBUG
        uart_string("Recovered from fault,turning On contactor.....!\r\n");
#endif
    FaultLEDOff();
    contactor_on();
    ISL_EnableReceiveCallback();
}
void recover_from_faults(void)
{
    Uint8 CurrentDevice;
    for(CurrentDevice=0;CurrentDevice<NumISLDevices;CurrentDevice++)
    {
        CheckFaults(CurrentDevice);
        while(AllFaults[CurrentDevice].IsFault == True)
        {
            clear_all_fault();
            DELAY_S(2);
            ISL_Request(CurrentDevice+1, READ_FAULTS);
            DELAY_S(3);
            CheckFaults(CurrentDevice);
            alert_ecu();
        }
    }
}
void clear_all_fault(void)
{
#ifdef DEBUG
        uart_string("In Fault Mode...!Clear all faults now...!\r\n");
#endif
    Uint8 Reset[8] = {},device;
    for(device = 1;device <= NumISLDevices; device++)
        ISL_WriteRegister(device,2,0x00,Reset);                                                      //reset all the pages one by one
    for(device = 1;device <= NumISLDevices; device++)
        ISL_WriteRegister(device,2,0x01,Reset);                                                      //reset all the pages one by one
    for(device = 1;device <= NumISLDevices; device++)
        ISL_WriteRegister(device,2,0x02,Reset);                                                      //reset all the pages one by one
    for(device = 1;device <= NumISLDevices; device++)
        ISL_WriteRegister(device,2,0x06,Reset);                                                      //reset all the pages one by one
    for(device = 1;device <= NumISLDevices; device++)
        ISL_WriteRegister(device,2,0x04,Reset);                                                      //reset all the pages one by one
}

#pragma CODE_SECTION(get_threshold_value,".bigCode")
Uint16 get_threshold_value(Uint8 fault_code)
{
    Uint16 Value;
    switch(fault_code)
    {
        case Fault_UV :
            return (Uint16)(((1<<13)/5)*UV_LIMIT);                                            //Set Lower Limit to 1.05

        case Fault_OV :
            return (Uint16)(((1<<13)/5)*OV_LIMIT);

        case Fault_OverTemp :
            Value = temp_lookup_table[(OT_LIMIT/5)-5][0];
            Value |= (temp_lookup_table[(OT_LIMIT/5)-5][1]) << 8;
            return Value;

        case Fault_OverCurrent :
            return OverCurrentThreshold;

        default:
            return 0xFFFF;
    }
}

#pragma CODE_SECTION(get_current_value,".bigCode")
Uint16 get_current_value(Uint8 FaultCode,Uint8 device,Uint8 cell_no)
{
    Uint16 currentValue;
    Uint8 buf[16]={};
    switch(FaultCode)
    {
        //fault UV
        case Fault_UV:
            ISL_ReadRegister(device, 0x01, cell_no);
            DELAY_MS(1);

#ifdef DEBUG
            uart_string("UV Dev= ");my_itoa(device, buf);uart_string(buf);
            uart_string(" Cell_no= ");my_itoa(cell_no, buf);uart_string(buf);
            uart_string(" Vltg now= ");
#endif
            currentValue = read_voltage(device, cell_no);
            double FloatValue = get_float_value_for_voltage(currentValue, cell);float_to_ascii(FloatValue, buf);
#ifdef DEBUG
            uart_string(buf);uart_string("\r\n");
#endif
            return currentValue;

        //fault_OV
        case Fault_OV:
            ISL_ReadRegister(device, 0x01, cell_no);
            DELAY_MS(1);
            currentValue = read_voltage(device, cell_no);
#ifdef DEBUG
            uart_string("OV Dev= ");my_itoa(device, buf);uart_string(buf);
            uart_string(" Cell_no= ");my_itoa(cell_no, buf);uart_string(buf);
            uart_string(" Vltg now= ");
            FloatValue = get_float_value_for_voltage(currentValue, cell);
            float_to_ascii(FloatValue, buf);
            uart_string(buf);uart_string("\r\n");
#endif
            return currentValue;

        //Fault_OverCurrent
        case Fault_OverCurrent:
            currentValue = GetNowCurrent();
            uart_string("OverCurrent");float_to_ascii((currentValue/4195.00), buf);uart_string(" I now=");
            uart_string_newline(buf);uart_string("A\r\n");
            return NowCurrent;

        //Fault OpenWire
        case Fault_OpenWire:
            ISL_ReadRegister(device, 3, 2);
            DELAY_MS(1);
#ifdef DEBUG
            uart_string("OW Dev=");my_itoa(device, buf);uart_string(buf);uart_string(" Reg Val now= ");
#endif
            ISL_DEVICE * ISL_Data = GetISLDevices(device);
            currentValue = ISL_Data->PAGE2_1.FAULT.OC.all;
#ifdef DEBUG
            my_itoa(currentValue, buf);uart_string(buf);uart_string("\r\n");
#endif
            return currentValue;

        //Overtemperature Fault
        case Fault_OverTemp:
            if(cell_no == 1)
            {
                ISL_ReadRegister(device, 0x01, 0x11);
                DELAY_MS(1);
                ISL_DEVICE * ISL_Data = GetISLDevices(device);
                currentValue = ISL_Data->PAGE1.TEMP.ET1V;
            }
            else if(cell_no == 2)
            {
                ISL_ReadRegister(device, 0x01, 0x12);
                DELAY_MS(1);
                ISL_DEVICE * ISL_Data = GetISLDevices(device);
                currentValue = ISL_Data->PAGE1.TEMP.ET2V;
            }
            if(cell_no == 3)
            {
                ISL_ReadRegister(device, 0x01, 0x13);
                DELAY_MS(1);
                ISL_DEVICE * ISL_Data = GetISLDevices(device);
                currentValue = ISL_Data->PAGE1.TEMP.ET3V;
            }
            if(cell_no == 4)
            {
                ISL_ReadRegister(device, 0x01, 0x14);
                DELAY_MS(1);
                ISL_DEVICE * ISL_Data = GetISLDevices(device);
                currentValue =  ISL_Data->PAGE1.TEMP.ET4V;
            }
#ifdef DEBUG
            uart_string("OvTemp dev= ");my_itoa(device, buf);uart_string(buf);
            uart_string(" Sens_No= ");my_itoa(cell_no, buf);uart_string(buf);
            uart_string(" Temp_now = ");
            double temperature = read_temp(device,cell_no);float_to_ascii(temperature, buf);
            uart_string(buf);uart_string("degreeC\r\n");
#endif
            return currentValue;

        //LowerSOC
        case Fault_Low_SOC:
#ifdef DEBUG
            uart_string("LowSOC");float_to_ascii(get_current_soc(), buf);
            uart_string(" SOCnow= ");uart_string(buf);uart_string("%\r\n");
#endif
            return  ((Uint16)(get_current_soc()/100.00) * 65535);

        //defaultValue
        default:
            return currentValue;
    }
}
void fill_can_message(Uint8 FaultCode,Uint8 device,Uint8 cell_no)
{
    Uint16 Value;

    //step1: fill the fault code here
    Fault_data[0] = FaultCode;

    //step2: fill device and cell_no
    Fault_data[1] = ((cell_no & 0x0f) << 4) | (device & 0x0f);

    //step3: fill the current value

    Value = get_current_value(FaultCode,device,cell_no);

    Fault_data[2] = 0xFF & Value;
    Fault_data[3] = 0xFF & ( Value >> 8);

    //step4: Fill threshold value
    Value = get_threshold_value(FaultCode);
    Fault_data[4] = (Value) & 0xFF;
    Fault_data[5] = (Value >> 8) & 0xFF;
}
