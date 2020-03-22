
/*
 *
 * SPI.c
 *
 * Author: Motivo Engineering
 * Copyright 2014
 *
 * Summary: Low level SPI driver for f2803x family TI DSP
 *
 */


#include "all_header.h"

SPI_RECEIVE_BUFFER SPI_ReceiveBuffer;
SPI_CALLBACKS SPICallbacks;
#ifndef _FLASH
#pragma CODE_SECTION(SPI_Init,".bigCode")
#endif
void SPI_Init() {

    unsigned char i;

    DisableISR();

    EALLOW;

    /* Enable internal pull-up for the selected pins */
    // Pull-ups can be enabled or disabled by the user.
    // This will enable the pullups for the specified pins.
    // Comment out other unwanted lines.

    GpioCtrlRegs.GPBPUD.bit.GPIO58 = 0;              // Enable pull-up on GPIO58 (SPISIMOA)
    GpioCtrlRegs.GPBPUD.bit.GPIO59 = 0;              // Enable pull-up on GPIO59 (SPISOMIA)
    GpioCtrlRegs.GPBPUD.bit.GPIO60 = 0;              // Enable pull-up on GPIO60 (SPICLKA)
    GpioCtrlRegs.GPBPUD.bit.GPIO61 = 0;              // Enable pull-up on GPIO61 (SPISTEA)

    GpioCtrlRegs.GPBQSEL2.bit.GPIO58 = 3;         // Asynch input GPIO58 (SPISIMOA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO59 = 3;         // Asynch input GPIO59 (SPISOMIA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO60 = 3;         // Asynch input GPIO60 (SPICLKA)
    GpioCtrlRegs.GPBQSEL2.bit.GPIO61 = 3;         // Asynch input GPIO61 (SPISTEA)

    GpioCtrlRegs.GPBGMUX2.bit.GPIO58 = 3;         // Configure GPIO58 as SPISIMOA
    GpioCtrlRegs.GPBGMUX2.bit.GPIO59 = 3;         // Configure GPIO59 as SPISOMIA
    GpioCtrlRegs.GPBGMUX2.bit.GPIO60 = 3;         // Configure GPIO60 as SPICLKA
    GpioCtrlRegs.GPBGMUX2.bit.GPIO61 = 3;         // Configure GPIO61 as SPISTEA

    GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 3;         // Configure GPIO58 as SPISIMOA
    GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 3;         // Configure GPIO59 as SPISOMIA
    GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 3;         // Configure GPIO60 as SPICLKA
    GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 3;         // Configure GPIO61 as SPISTEA

    EDIS;

    EALLOW;
    PieVectTable.XINT1_INT = &SPI_DRDY_ISR;
    EDIS;

    // Enable XINT1 and XINT2 in the PIE: Group 1 interrupt 4 & 5
    // Enable INT1 which is connected to WAKEINT:
    // may need to ACK interrupts after ENPIE. look in PIE example code
    PieCtrlRegs.PIECTRL.bit.ENPIE = 1;          // Enable the PIE block
    PieCtrlRegs.PIEIER1.bit.INTx4 = 1;          // Enable PIE Gropu 1 INT4 XINT1
    IER |= M_INT1;                              // Enable CPU INT1

    // Set SPI controls
    SpiaRegs.SPICCR.all =0x0007;          // Reset on, rising edge xmit, 8-bit char bits
    SpiaRegs.SPICTL.all =0x000E;          // Enable master mode, normal 1.5 phase,
                                          // enable talk, and SPI int disabled.
    SpiaRegs.SPIBRR.all =0x000B;              // divide by 11+1 was  50/12 ~ 4MHz
    SpiaRegs.SPICCR.all =0x0087;          // Relinquish SPI from Reset
    SpiaRegs.SPIPRI.bit.FREE = 1;         // Set so breakpoints don't disturb xmission

    // Set callback for timeout
    SPI_SetTimerCallback(SPI_TMR_ISR);

    // Disable them because they are not initialized
    SPI_DisableDRDYCallback();
    SPI_DisableTimeoutCallback();

    // Clear receive buffer
    SPI_ReceiveBuffer.numBytes = 0;
    for(i=0;i<SPI_MAX_READ_BYTES;i++) {
        SPI_ReceiveBuffer.data[i] = 0xFF;
    }

    // GPIO1 is input for DRDY Pin
    EALLOW;
    GpioCtrlRegs.GPAMUX1.bit.GPIO1 = 0;         // GPIO
    GpioCtrlRegs.GPADIR.bit.GPIO1 = 0;          // input
    GpioCtrlRegs.GPAQSEL1.bit.GPIO1 = 0;        // XINT1 Synch to SYSCLKOUT only
    GpioCtrlRegs.GPACTRL.bit.QUALPRD0 = 0xFF;   // Sampling window is 510*SYSCLKOUT
    EDIS;

    GPIO_SetupXINT1Gpio(1);

    // Configure XINT1
    XintRegs.XINT1CR.bit.POLARITY = 0;      // Falling edge interrupt

    // Enable XINT1
    XintRegs.XINT1CR.bit.ENABLE = 1;        // Enable XINT1

    EnableISR();
#ifdef DEBUG
    uart_string("SPI Setup Complete!\r\n");
#endif
}

void SPI_Test() {

    // Setup timers, callbacks, and interrupts
    //TMR_Init();

    //SPI_Init();

    //SPI_SetTimeout(0x1FFF);

    //SPI_SetDRDYCallback(SPI_TestDRDYCallback);
    //SPI_SetTimerCallback(SPI_TestTimerCallback);

    //SPI_EnableDRDYCallback();
    //SPI_EnableTimeoutCallback();

    // Send some data
    // Bogus data should elicit some response

    unsigned char read_data[4]={};
    unsigned char test;
    unsigned char delay = 3;

    while(1)
    {
    test = 0x03;
    SPI_Write(&test);
    DELAY_US(delay);

    test = 0x24;
    SPI_Write(&test);
    DELAY_US(delay);

    test = 0x04;
    SPI_Write(&test);
    DELAY_US(delay);

    while(GpioDataRegs.GPADAT.bit.GPIO1 == 1);

    SPI_Read(&read_data[0]);
    DELAY_US(delay);

    while(GpioDataRegs.GPADAT.bit.GPIO1 == 1);
    SPI_Read(&read_data[1]);
    DELAY_US(delay);

    while(GpioDataRegs.GPADAT.bit.GPIO1 == 1);
    SPI_Read(&read_data[2]);
    DELAY_US(delay);

    while(GpioDataRegs.GPADAT.bit.GPIO1 == 1);
    SPI_Read(&read_data[3]);
    DELAY_US(delay);

    }
    // hopefully DRDY toggles
}

void SPI_TestDRDYCallback() {

    unsigned char i;
    unsigned char j;
    for(i=0;i<256;i++) { j++; }
}

void SPI_TestTimerCallback() {

    unsigned char i;
    unsigned char j;
    for(i=0;i<256;i++) { j++; }
}

// additional operations after grabbing data or timing out
void SPI_SetDRDYCallback(void (*callback)()) {

    SPICallbacks.callback[0] = callback;
}

void SPI_EnableDRDYCallback() {

    SPICallbacks.enable[0] = True;
}

void SPI_DisableDRDYCallback() {

    SPICallbacks.enable[0] = False;
}

void SPI_SetTimeoutCallback(void (*callback)()) {

    SPICallbacks.callback[1] = callback;
}

void SPI_EnableTimeoutCallback() {

    SPICallbacks.enable[1] = True;
}

void SPI_DisableTimeoutCallback() {

    SPICallbacks.enable[1] = False;
}

// interrupt routines
// interrupt is called on DRDY edge
__interrupt void SPI_DRDY_ISR() {

    // Reset the timer
    //SPI_ResetTimer();

    // Read in the byte
    SPI_Read(SPI_ReceiveBuffer.data+SPI_ReceiveBuffer.numBytes);
    SPI_ReceiveBuffer.numBytes++;

    // Process the callback
    if(SPICallbacks.enable[0] == True) { (*(SPICallbacks.callback[0]))(); }

    // Acknowledge this interrupt to get more from group 1
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;
}

// interrupt is called on TIMER timeout
void SPI_TMR_ISR() {

    // Stop the timer
    SPI_StopTimer();

    // Process the callback
    if(SPICallbacks.enable[1] == True) { (*(SPICallbacks.callback[1]))(); }

    // Clear the buffer
    SPI_ReceiveBuffer.numBytes = 0;
}

// manually timeout
void SPI_ForceTimeout() {

    // Stop the timer
    SPI_StopTimer();

    // Process the callback
    if(SPICallbacks.enable[1] == True) { (*(SPICallbacks.callback[1]))(); }
}

// timer routines
void SPI_SetTimerCallback(void (*callback)()) {

    TMR_SetTimeoutCallback(GNR_SPI_TIMER, callback);
}

void SPI_StartTimer() {

    TMR_StartTimer(GNR_SPI_TIMER);
}

void SPI_ResetTimer() {

    TMR_ResetTimer(GNR_SPI_TIMER);
}

void SPI_StopTimer() {

    TMR_StopTimer(GNR_SPI_TIMER);
}

void SPI_SetTimeout(Uint16 uS) {

    TMR_SetTimeout(GNR_SPI_TIMER, uS);
}

// data read routines
SPI_RECEIVE_BUFFER* SPI_GetReadBuffer() {

    return &SPI_ReceiveBuffer;
}

Bool SPI_Read(unsigned char* buff) {

    volatile unsigned char tmp;

    SpiaRegs.SPICTL.bit.CLK_PHASE = 0;

    // Bring chip select low
    GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;   // Clear output latch and pad
    DELAY_US(SPI_CS_PRE_PADDING);

    // Read until interrupt is clear
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 1 ) { tmp = SpiaRegs.SPIRXBUF; }

    // Xmit 8 bits of dummy data
    SpiaRegs.SPITXBUF=0x0;

    // Wait for received data
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0) { }

    SpiaRegs.SPICTL.bit.CLK_PHASE = 1;

    // Store received data
    *buff = SpiaRegs.SPIRXBUF;

    // Bring chip select high
    DELAY_US(SPI_CS_POST_PADDING);
    GpioDataRegs.GPBSET.bit.GPIO61 = 1;   // Pad and set output latch

    return True;
}


// data write routines
Bool SPI_Write(unsigned char* buff) {

    volatile unsigned char tmp;

    DisableISR();

    // Bring chip select low
    GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;   // Clear output latch and pad
    DELAY_US(SPI_CS_PRE_PADDING);

    // Xmit 8 bits of data
    SpiaRegs.SPITXBUF=(((Uint16)(*buff))<<8);

    // Wait for data to be loaded
    while(SpiaRegs.SPISTS.bit.BUFFULL_FLAG ==1) { }

    // Wait for data to be send
    while(SpiaRegs.SPISTS.bit.INT_FLAG == 0) { }

    // Clear interrupt flag
    tmp = SpiaRegs.SPIRXBUF;

    // Bring chip select high
    DELAY_US(SPI_CS_POST_PADDING);
    GpioDataRegs.GPBSET.bit.GPIO61 = 1;   // Pad and set output latch

    EnableISR();

    // Start timer
    SPI_StartTimer();

    return True;
}


