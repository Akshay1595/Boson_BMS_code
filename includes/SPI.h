/*
 *
 * SPI.h
 *
 * Author: Jama Mohamed
 *
 * Summary: Low level SPI driver for f2803x family TI DSP
 *
 * Modified:
 *
 *           Added changelist   7/1/2014 JAM
 *
 */

#ifndef SPI_H_
#define SPI_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "General.h"


/** OLD SPI
unsigned char* SPI_GetReadBuffer();
void SPI_Init();
void SPI_Start_Timer();
void SPI_Stop_Timer();
void SPI_Read(unsigned char* buff, Uint16 len); // should only read after DRDY asserted
void SPI_Dummy_Read(Uint16 len); // reads data but doesnt overwrite buffer
void SPI_Write(unsigned char* buff, Uint16 len, Uint16 expected_len, unsigned char dummy);
void SPI_Wait(); // BLOCKING // waits for return to be serviced. simply checks bit which will be 'done' after return interrupts collect all bytes expected
__interrupt void SPI_DRDY_ISR(void); // interrupt is called on DRDY edge
__interrupt void SPI_TMR_ISR(void); // interrupt is called on TIMER timeout
**/

#define SPI_MAX_READ_BYTES 128
#define SPI_CS_PRE_PADDING  1    // in uS
#define SPI_CS_POST_PADDING 1    // in uS

#define SPI_MAX_NUM_CALLBACK 2

typedef struct {
    unsigned char data[SPI_MAX_READ_BYTES];
    unsigned char numBytes;
} SPI_RECEIVE_BUFFER;

typedef struct {
    void (*callback[SPI_MAX_NUM_CALLBACK])();
    Bool enable[SPI_MAX_NUM_CALLBACK];
} SPI_CALLBACKS;

// Initialization
void SPI_Init();
void SPI_Test();
void SPI_TestDRDYCallback();
void SPI_TestTimerCallback();
void InitSpia2Gpio(void);

// Additional operations after grabbing data or timing out
void SPI_SetDRDYCallback(void (*callback)());
void SPI_EnableDRDYCallback();
void SPI_DisableDRDYCallback();

void SPI_SetTimeoutCallback(void (*callback)());
void SPI_EnableTimeoutCallback();
void SPI_DisableTimeoutCallback();

// Interrupt routines
__interrupt void SPI_DRDY_ISR(); // interrupt is called on DRDY edge
void SPI_TMR_ISR(); // interrupt is called on TIMER timeout

// Timer routines // uses timer 0
void SPI_StartTimer();
void SPI_ResetTimer();
void SPI_StopTimer();
void SPI_ForceTimeout();
void SPI_SetTimeout(Uint16 uS);
void SPI_SetTimerCallback(void (*callback)());

// Data read routines
SPI_RECEIVE_BUFFER* SPI_GetReadBuffer();
Bool SPI_Read(unsigned char* buff);

// Data write routines
Bool SPI_Write(unsigned char* buff);

#endif /* SPI_H_ */
