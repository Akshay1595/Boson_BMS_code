/*
 *
 * ISL94212.h
 *
 * Author: Jama Mohamed
 *
 * Summary: Routines to interface with Intersil BMS via SPI
 *
 * Modified:
 *
 * 			 Added changelist   7/1/2014 JAM
 * 			 Added NumDevices Proto
 *
 */


// WORRIES:

// Reference Coefficient Registers
//struct REF_COEFF {              // bits description
//	Uint16 COEFF_C;             // Reference Coefficient C
//	Uint16 COEFF_B;             // Reference Coefficient B
//	Uint16 rsvd:5;              // reserved
//	Uint16 COEFF_A:11;          // Reference Coefficient A
//};
//
// I am worried that COEFF_A will not pack correctly which will mess up the packing of all the words following



#ifndef ISL94212_H_
#define ISL94212_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "SPI.h"

#define ISL_MAX_DEVICES 14 // Maximum number of ISL devices
#define ISL_MAX_NUM_CALLBACK 1

// Defines used to figure out which sub-page data is in and offset
// 3' page, 6' addr

// Page 1 - Just needs to say Page 1                                 so 0b001xxxxxx
#define ISL_PAGE1_START_ADDR  0x040              // Value to match against after masking
#define ISL_PAGE1_START_MASK  0x1C0              // Mask value to AND with
#define ISL_PAGE1_OFFSET_MASK 0x1F               // Mask value to grab offset location
#define ISL_PAGE1_LENGTH sizeof(struct PAGE1)    // Limit offset so that we done overflow // returns # of Uint16? wtf

// Page 2_1 - Needs to say Page 2 AND upper bits 0b00                so 0b01000xxxx
#define ISL_PAGE2_1_START_ADDR 0x080
#define ISL_PAGE2_1_START_MASK 0x1F0
#define ISL_PAGE2_1_OFFSET_MASK 0x07
#define ISL_PAGE2_1_LENGTH sizeof(struct PAGE2_1)

// Page 2_2 - Needs to say Page 2 AND upper bits 0b01                so 0b01001xxxx
#define ISL_PAGE2_2_START_ADDR 0x090
#define ISL_PAGE2_2_START_MASK 0x1F0
#define ISL_PAGE2_2_OFFSET_MASK 0x0F
#define ISL_PAGE2_2_LENGTH sizeof(struct PAGE2_2)

// Page 2_3 - Needs to say Page 2 AND upper bits 0b1x (0b10 or 0b11) so 0b0101xxxxx
#define ISL_PAGE2_3_START_ADDR 0x0A0
#define ISL_PAGE2_3_START_MASK 0x1E0
#define ISL_PAGE2_3_OFFSET_MASK 0x1F
#define ISL_PAGE2_3_LENGTH sizeof(struct PAGE2_3)

// Page 3 - Commands - Check ISL_COMMAND enum

// Page 4 - Special register
// not implemented

// Page 5 - Special register
// not implemented

#define ISL_COMMAND_PADDING  63000    // in uS // min time between commands
#define ISL_COMMAND_TIMEOUT  60000    // in uS // wait time for command response
#define ISL_BYTE_PADDING     10      // in uS


// must be less than ISL_COMMAND_PADDING
#if ISL_COMMAND_TIMEOUT > ISL_COMMAND_PADDING
    omg dont compile me!!!
#endif

//============================= System ==================================

typedef struct {
	void (*callback[ISL_MAX_NUM_CALLBACK])();
	Bool enable[ISL_MAX_NUM_CALLBACK];
} ISL_CALLBACKS;

//============================= Device ==================================

#define ISL_COMMAND_BYTES 5

typedef struct {
	Uint8 byte1;
	Uint8 byte2;
	Uint8 byte3;
	Uint8 byte4;
	Uint8 numBytes;
} ISL_PACKET;


typedef enum {

	UNKNOWN,
	IDENTIFYING,
	READY
} ISL_STATE;

typedef struct {

	Uint8 guessValid;
	Uint8 bytesExpected;
} ISL_COMMAND_GUESS;

typedef struct {

	Bool newResponse;
	Bool newData;
	Bool newIdentify;
	Bool newAck;
	Bool nak;
	Bool timeout;
	Bool tableError;
	Uint8 Device;
	Uint8 Header;

} ISL_FLAGS;


//============================ Commands =================================

// 3' page, 6' addr
typedef enum {
	SCAN_VOLTAGES = 0x0C1,
	SCAN_TEMPERATURES = 0x0C2,
	SCAN_MIXED = 0x0C3,
	SCAN_WIRES = 0x0C4,
	SCAN_ALL = 0x0C5,
	SCAN_CONTINOUS = 0x0C6,
	SCAN_INHIBIT = 0x0C7,
	MEASURE = 0x0C8,
	IDENTIFY = 0x0C9,
	SLEEP = 0x0CA,
	NAK = 0x0CB,
	ACK = 0x0CC,
	COMMS_FAILURE = 0x0CE,
	WAKEUP = 0x0CF,
	BALANCE_ENABLE = 0x0D0,
	BALANCE_INHIBIT = 0x0D1,
	RESET = 0x0D2,
	CALCULATE_REGISTER_CHECKSUM = 0x0D3,
	CHECK_REGISTER_CHECKSUM = 0x0D4,

	READ_VOLTAGES = 0x04F,
	READ_TEMP = 0x05F,
	BALANCE_STATUS =0x94,
	READ_FAULTS=0x8F
} ISL_REQUEST;

//============================= Page 1 ==================================

// Cell Voltages
struct CELL_VOLTAGES {     // bits description
   Uint16 VB;              //      Stack voltage
   Uint16 C1V;             //      Cell voltage 1
   Uint16 C2V;             //      Cell voltage 2
   Uint16 C3V;             //      Cell voltage 3
   Uint16 C4V;             //      Cell voltage 4
   Uint16 C5V;             //      Cell voltage 5
   Uint16 C6V;             //      Cell voltage 6
   Uint16 C7V;             //      Cell voltage 7
   Uint16 C8V;             //      Cell voltage 8
   Uint16 C9V;             //      Cell voltage 9
   Uint16 C10V;            //      Cell voltage 10
   Uint16 C11V;            //      Cell voltage 11
   Uint16 C12V;            //      Cell voltage 12
};

// Scan Count
struct SCAN_COUNT_BITS {   // bits description
   Uint16 SCN0:1;          // 0    Acknowledge PIE interrupt group 1
   Uint16 SCN1:1;          // 1    Acknowledge PIE interrupt group 2
   Uint16 SCN2:1;          // 2    Acknowledge PIE interrupt group 3
   Uint16 SCN3:1;          // 3    Acknowledge PIE interrupt group 4
   Uint16 rsvd:12;         // 15:4 reserved
};

union SCAN_COUNT {
   Uint16                  all;
   struct SCAN_COUNT_BITS  bit;
};

// Temperatures
struct EXT_TEMPERATURES {  // bits description
   Uint16           ICT;   //       IC Temperature
   Uint16           ET1V;  //       External Temperature 1
   Uint16           ET2V;  //       External Temperature 2
   Uint16           ET3V;  //       External Temperature 3
   Uint16           ET4V;  //       External Temperature 4
   Uint16           RV;    //       Secondary Reference Voltage
   union SCAN_COUNT SCN;   //       Scan Count
};


// Page 1
struct PAGE1 {                     // bits description
   struct CELL_VOLTAGES    CELLV;  // Voltages
   Uint16                  rsvd0;  // reserved
   Uint16                  rsvd1;  // reserved
   Uint16                  rsvd2;  // reserved
   struct EXT_TEMPERATURES TEMP;   // Temperatures
};

//============================= Page 2_1 ================================

// Cell Faults
struct CELL_FAULT_BITS {   // bits description
   Uint16 FB1:1;           // 0     Cell Fault Bit 1
   Uint16 FB2:1;           // 1     Cell Fault Bit 2
   Uint16 FB3:1;           // 2     Cell Fault Bit 3
   Uint16 FB4:1;           // 3     Cell Fault Bit 4
   Uint16 FB5:1;           // 4     Cell Fault Bit 5
   Uint16 FB6:1;           // 5     Cell Fault Bit 6
   Uint16 FB7:1;           // 6     Cell Fault Bit 7
   Uint16 FB8:1;           // 7     Cell Fault Bit 8
   Uint16 FB9:1;           // 8     Cell Fault Bit 9
   Uint16 FB10:1;          // 9     Cell Fault Bit 10
   Uint16 FB11:1;          // 10    Cell Fault Bit 11
   Uint16 FB12:1;          // 11    Cell Fault Bit 12
   Uint16 rsvd:4;          // 15:12 reserved
};

union CELL_FAULT {
   Uint16                  all;
   struct CELL_FAULT_BITS  bit;
};

// Wire Faults
struct WIRE_FAULT_BITS {   // bits description
   Uint16 OW0:1;           // 0     Wire Fault Bit 0
   Uint16 OW1:1;           // 1     Wire Fault Bit 1
   Uint16 OW2:1;           // 2     Wire Fault Bit 2
   Uint16 OW3:1;           // 3     Wire Fault Bit 3
   Uint16 OW4:1;           // 4     Wire Fault Bit 4
   Uint16 OW5:1;           // 5     Wire Fault Bit 5
   Uint16 OW6:1;           // 6     Wire Fault Bit 6
   Uint16 OW7:1;           // 7     Wire Fault Bit 7
   Uint16 OW8:1;           // 8     Wire Fault Bit 8
   Uint16 OW9:1;           // 9     Wire Fault Bit 9
   Uint16 OW10:1;          // 10    Wire Fault Bit 10
   Uint16 OW11:1;          // 11    Wire Fault Bit 11
   Uint16 OW12:1;          // 12    Wire Fault Bit 12
   Uint16 rsvd:3;          // 15:13 reserved
};

union WIRE_FAULT {
   Uint16                  all;
   struct WIRE_FAULT_BITS  bit;
};

// Fault Setup
struct FAULT_SETUP_BITS {  // bits description
   Uint16 SCN0:1;          // 0     Scan Interval Code 0
   Uint16 SCN1:1;          // 1     Scan Interval Code 1
   Uint16 SCN2:1;          // 2     Scan Interval Code 2
   Uint16 SCN3:1;          // 3     Scan Interval Code 3
   Uint16 WSCN:1;          // 4     Scan Wires Timing Control
   Uint16 TOT0:1;          // 5     Fault Totalize Code Bit 0
   Uint16 TOT1:1;          // 6     Fault Totalize Code Bit 1
   Uint16 TOT2:1;          // 7     Fault Totalize Code Bit 2
   Uint16 TTST0:1;         // 8     Internal Temperature Testing Control
   Uint16 TTST1:1;         // 9     External Temperature Testing Control Bit 0
   Uint16 TTST2:1;         // 10    External Temperature Testing Control Bit 0
   Uint16 TTST3:1;         // 11    External Temperature Testing Control Bit 0
   Uint16 TTST4:1;         // 12    External Temperature Testing Control Bit 0
   Uint16 rsvd:3;          // 15:13 reserved
};

union FAULT_SETUP {
   Uint16                   all;
   struct FAULT_SETUP_BITS  bit;
};

// Fault Status
struct FAULT_STATUS_BITS { // bits description
   Uint16 zero:2;          // 1:0   zero
   Uint16 OSC:1;           // 2     Oscillator Fault
   Uint16 WDGF:1;          // 3     Wachdog Timeout Fault
   Uint16 OT:1;            // 4     Over Temperature Fault
   Uint16 OV:1;            // 5     Overvoltage Fault
   Uint16 UV:1;            // 6     Undervoltage Fault
   Uint16 OW:1;            // 7     Open Wire Fault
   Uint16 OVBAT:1;         // 8     Open Wire Fault on Vbat
   Uint16 OVSS:1;          // 9     Open Wire Fault on Vss
   Uint16 PAR:1;           // 10    Register Checksum(parity) Error
   Uint16 REF:1;           // 11    Voltage Reference Fault
   Uint16 REG:1;           // 12    Voltage Regulator Fault
   Uint16 MUX:1;           // 13    Temperature Multiplexer Error
   Uint16 rsvd:2;          // 15:14 reserved
};

union FAULT_STATUS {
   Uint16                    all;
   struct FAULT_STATUS_BITS  bit;
};

// Cell Setup
struct CELL_SETUP_BITS {   // bits description
   Uint16 C1:1;            // 0     Cell OV/UV Detection Enable 1
   Uint16 C2:1;            // 1     Cell OV/UV Detection Enable 2
   Uint16 C3:1;            // 2     Cell OV/UV Detection Enable 3
   Uint16 C4:1;            // 3     Cell OV/UV Detection Enable 4
   Uint16 C5:1;            // 4     Cell OV/UV Detection Enable 5
   Uint16 C6:1;            // 5     Cell OV/UV Detection Enable 6
   Uint16 C7:1;            // 6     Cell OV/UV Detection Enable 7
   Uint16 C8:1;            // 7     Cell OV/UV Detection Enable 8
   Uint16 C9:1;            // 8     Cell OV/UV Detection Enable 9
   Uint16 C10:1;           // 9     Cell OV/UV Detection Enable 10
   Uint16 C11:1;           // 10    Cell OV/UV Detection Enable 11
   Uint16 C12:1;           // 11    Cell OV/UV Detection Enable 12
   Uint16 FFSP:1;          // 12    Force ADC Full Scale Positive
   Uint16 FFSN:1;          // 13    Force ADC Full Scale Negative
   Uint16 rsvd:2;          // 15:14 reserved
};

union CELL_SETUP {
   Uint16                    all;
   struct CELL_SETUP_BITS    bit;
};

// Over Temperature Faults
struct OVERT_FAULT_BITS {   // bits description
   Uint16 TFLT0:1;          // 0     Internal Over Temperature Fault
   Uint16 TFLT1:1;          // 1     External Over Temperature Fault 0
   Uint16 TFLT2:1;          // 2     External Over Temperature Fault 1
   Uint16 TFLT3:1;          // 3     External Over Temperature Fault 2
   Uint16 TFLT4:1;          // 4     External Over Temperature Fault 3
   Uint16 rsvd:11;          // 15:5 reserved
};

union OVERT_FAULT {
   Uint16                    all;
   struct OVERT_FAULT_BITS   bit;
};

// Faults
struct FAULTS {             // bits description
   union CELL_FAULT   OF;   // Over Voltage Fault
   union CELL_FAULT   UF;   // Under Voltage Fault
   union WIRE_FAULT   OC;   // Open Wire Fault
   union FAULT_SETUP  FSET; // Fault Setup
   union FAULT_STATUS FSTA; // Fault Status
   union CELL_SETUP   CSET; // Cell Setup
   union OVERT_FAULT  OVTF; // Over Temperature Fault
};

// Page 2_1
struct PAGE2_1 {            // bits description
   struct FAULTS FAULT;     // Faults
   Uint16        rsvd0;     // reserved
};


//============================= Page 2_2 ================================


// Balance Setup
struct BALANCE_SETUP_BITS { // bits description
   Uint16 MBD0:1;           // 0     Balance Mode 0
   Uint16 BMD1:1;           // 1     Balance Mode 1
   Uint16 BWT0:1;           // 2     Balance Wait Time 0
   Uint16 BWT1:1;           // 3     Balance Wait Time 1
   Uint16 BWT2:1;           // 4     Balance Wait Time 2
   Uint16 BSP0:1;           // 5     Balance Status Register Pointer 0
   Uint16 BSP1:1;           // 6     Balance Status Register Pointer 1
   Uint16 BSP2:1;           // 7     Balance Status Register Pointer 2
   Uint16 BSP3:1;           // 8     Balance Status Register Pointer 3
   Uint16 BEN:1;            // 9     Balance Enable
   Uint16 rsvd:6;           // 15:10 reserved
};

union BALANCE_SETUP {
   Uint16                    all;
   struct BALANCE_SETUP_BITS bit;
};

// Balance Status
struct BALANCE_STATUS_BITS { // bits description
   Uint16 BAL1:1;            // 0     Balance Status 1
   Uint16 BAL2:1;            // 1     Balance Status 2
   Uint16 BAL3:1;            // 2     Balance Status 3
   Uint16 BAL4:1;            // 3     Balance Status 4
   Uint16 BAL5:1;            // 4     Balance Status 5
   Uint16 BAL6:1;            // 5     Balance Status 6
   Uint16 BAL7:1;            // 6     Balance Status 7
   Uint16 BAL8:1;            // 7     Balance Status 8
   Uint16 BAL9:1;            // 8     Balance Status 9
   Uint16 BAL10:1;           // 9     Balance Status 10
   Uint16 BAL11:1;           // 10    Balance Status 11
   Uint16 BAL12:1;           // 11    Balance Status 12
   Uint16 rsvd:4;            // 15:12 reserved
};

union BALANCE_STATUS {
   Uint16                     all;
   struct BALANCE_STATUS_BITS bit;
};


// Balance Time
struct BALANCE_TIME_BITS {   // bits description
   Uint16 WDG0:1;            // 0     Watchdog Timeout 1
   Uint16 WDG1:1;            // 1     Watchdog Timeout 2
   Uint16 WDG2:1;            // 2     Watchdog Timeout 3
   Uint16 WDG3:1;            // 3     Watchdog Timeout 4
   Uint16 WDG4:1;            // 4     Watchdog Timeout 5
   Uint16 WDG5:1;            // 5     Watchdog Timeout 6
   Uint16 WDG6:1;            // 6     Watchdog Timeout 7
   Uint16 BTM0:1;            // 7     Watchdog Timeout 8
   Uint16 BTM1:1;            // 8     Watchdog Timeout 9
   Uint16 BTM2:1;            // 9     Watchdog Timeout 10
   Uint16 BTM3:1;            // 10    Watchdog Timeout 11
   Uint16 BTM4:1;            // 11    Watchdog Timeout 12
   Uint16 BTM5:1;            // 12    Watchdog Timeout 13
   Uint16 BTM6:1;            // 13    Watchdog Timeout 14
   Uint16 rsvd:2;            // 15:14 reserved
};

union BALANCE_TIME {
   Uint16                   all;
   struct BALANCE_TIME_BITS bit;
};

// Comms Setup
struct COMMS_SETUP_BITS {    // bits description
   Uint16 ADDR0:1;           // 0     Device Stack Address 1
   Uint16 ADDR1:1;           // 1     Device Stack Address 2
   Uint16 ADDR2:1;           // 2     Device Stack Address 3
   Uint16 ADDR3:1;           // 3     Device Stack Address 4
   Uint16 SIZE0:1;           // 4     Device Stack Size 1
   Uint16 SIZE1:1;           // 5     Device Stack Size 2
   Uint16 SIZE2:1;           // 6     Device Stack Size 3
   Uint16 SIZE3:1;           // 7     Device Stack Size 4
   Uint16 CSEL1:1;           // 8     Communications Setup 1
   Uint16 CSEL2:1;           // 9     Communications Setup 2
   Uint16 CRAT0:1;           // 10    Communications Rate 1
   Uint16 CRAT1:1;           // 11    Communications Rate 2
   Uint16 rsvd:4;            // 15:12 reserved
};

union COMMS_SETUP {
   Uint16                   all;
   struct COMMS_SETUP_BITS  bit;
};

// Device Setup
struct DEV_SETUP_BITS {    // bits description
   Uint16 PIN39:1;         // 0     Pin 39 Input
   Uint16 PIN37:1;         // 1     Pin 37 Input
   Uint16 rsvd0:1;         // 2     reserved
   Uint16 EOB:1;           // 3     End of Balance
   Uint16 SCAN:1;          // 4     Scan Continuous Mode
   Uint16 ISCN:1;          // 5     Wire Scan Value
   Uint16 rsvd1:1;         // 6     reserved
   Uint16 BDDS:1;          // 7     Balance Condition
   Uint16 WP0:1;           // 8     Watchdog Disable Password 1
   Uint16 WP1:1;           // 9     Watchdog Disable Password 2
   Uint16 WP2:1;           // 10    Watchdog Disable Password 3
   Uint16 WP3:1;           // 11    Watchdog Disable Password 4
   Uint16 WP4:1;           // 12    Watchdog Disable Password 5
   Uint16 WP5:1;           // 13    Watchdog Disable Password 6
   Uint16 rsvd2:2;         // 15:14 reserved
};

union DEV_SETUP {
   Uint16                 all;
   struct DEV_SETUP_BITS  bit;
};

// Hardware Serial Number
struct SERIAL {            // bits description
	Uint16 SER_LO;         // Hardware Serial Number Low
	Uint16 SE_HI;          // Hardware Serial Number High
};

// Trim Voltages
struct VTRIM_BITS {        // bits description
   Uint16 rsvd0:8;         // 7:0   reserved
   Uint16 TV:6;            // 13:8  Trim voltage
   Uint16 rsvd2:2;         // 15:14 reserved
};

union VTRIM {
   Uint16             all;
   struct VTRIM_BITS  bit;
};

// Setup
struct SETUP {                        // bits description
	Uint16               OV;          // Over Voltage Limit
	Uint16               UV;          // Under Voltage Limit
	Uint16               ETL;         // External Temperature Limit
	union BALANCE_SETUP  BSET;        // Balance Setup
	union BALANCE_STATUS BSTAT;       // Balance Status
	union BALANCE_TIME   BTIME;       // Balance Time/Watchdog
	Uint16               USR1;        // User Register 1
	Uint16               USR2;        // User Register 2
	union COMMS_SETUP    CSET;        // Comms Setup
	union DEV_SETUP      DSET;        // Device Setup
	Uint16               INTT_LIMIT;  // Internal Temperature Limit
	struct SERIAL        SER;         // Hardware Serial Number
	union VTRIM          VTRIM;       // Trim Voltage Reading
};

// Page 2_2
struct PAGE2_2 {                  // bits description
   struct SETUP        SETUP;     // Setup Registers
};

//============================= Page 2_3 ================================

// Cell Balance Words
struct CELL_BALANCE_BITS {            // bits description
   Uint16 BAL_LO;                     // 15:0   Balance SOC Value Low Word
   Uint16 BAL_HI;                     // 31:16  Balance SOC Value High Word
};

// Cell Balance Registers
struct CELL_BALANCE {                 // bits description
	struct CELL_BALANCE_BITS CELL1;   // Balance SOC Value Cell 1
	struct CELL_BALANCE_BITS CELL2;   // Balance SOC Value Cell 2
	struct CELL_BALANCE_BITS CELL3;   // Balance SOC Value Cell 3
	struct CELL_BALANCE_BITS CELL4;   // Balance SOC Value Cell 4
	struct CELL_BALANCE_BITS CELL5;   // Balance SOC Value Cell 5
	struct CELL_BALANCE_BITS CELL6;   // Balance SOC Value Cell 6
	struct CELL_BALANCE_BITS CELL7;   // Balance SOC Value Cell 7
	struct CELL_BALANCE_BITS CELL8;   // Balance SOC Value Cell 8
	struct CELL_BALANCE_BITS CELL9;   // Balance SOC Value Cell 9
	struct CELL_BALANCE_BITS CELL10;  // Balance SOC Value Cell 10
	struct CELL_BALANCE_BITS CELL11;  // Balance SOC Value Cell 11
	struct CELL_BALANCE_BITS CELL12;  // Balance SOC Value Cell 12
};


// Reference Coefficient Registers
struct REF_COEFF {              // bits description
	Uint16 COEFF_C;             // Reference Coefficient C
	Uint16 COEFF_B;             // Reference Coefficient B
	Uint16 rsvd:5;              // reserved
	Uint16 COEFF_A:11;          // Reference Coefficient A
};

// Cell In Balance Bits
struct CELL_INBAL_BITS {        // bits description
	Uint16 CBEN1:1;             // 0  Cell In Balance 1
	Uint16 CBEN2:1;             // 1  Cell In Balance 2
	Uint16 CBEN3:1;             // 2  Cell In Balance 3
	Uint16 CBEN4:1;             // 3  Cell In Balance 4
	Uint16 CBEN5:1;             // 4  Cell In Balance 5
	Uint16 CBEN6:1;             // 5  Cell In Balance 6
	Uint16 CBEN7:1;             // 6  Cell In Balance 7
	Uint16 CBEN8:1;             // 7  Cell In Balance 8
	Uint16 CBEN9:1;             // 8  Cell In Balance 9
	Uint16 CBEN10:1;            // 9  Cell In Balance 10
	Uint16 CBEN11:1;            // 10 Cell In Balance 11
	Uint16 CBEN12:1;            // 11 Cell In Balance 12
    Uint16 rsvd2:2;             // 15:14 reserved
};

// Cell In Balance Register
union CELL_INBAL {                // bits description
	Uint16                  all;
	struct CELL_INBAL_BITS  bit;
};

// Page 2_3
struct PAGE2_3 {                  // bits description
   struct CELL_BALANCE CBAL;      // Cell Balance Registers
   struct REF_COEFF    COEFF;     // Reference Coefficient Registers
   union CELL_INBAL    INBAL;     // Cell In Balance Register
};

// Entire Device
// Note: Words within Pages are continuous and mapped true to the device
typedef struct {                  // bits description
   struct PAGE1   PAGE1;          // Page 1
   struct PAGE2_1 PAGE2_1;        // Page 2_1
   struct PAGE2_2 PAGE2_2;        // Page 2_2
   struct PAGE2_3 PAGE2_3;        // Page 2_3
} ISL_DEVICE;


/**
Uint8 ISL_Init();
Uint16 ISL_ResetComms();
void ISL_FillCRC(Uint8* message);
void ISL_FillCRC4(Uint8* message);
void ISL_Reset();
Uint8 ISL_Identify(Uint8 device);
Uint16 ISL_ReadReg(Uint8 deviceAddr, Uint8 pageAddr, Uint8 dataAddr, Uint8 waitValue);
Uint16 ISL_WriteReg(Uint8 deviceAddr, Uint8 pageAddr, Uint8 dataAddr, Uint16 data, Uint8 waitValue);
float32* ISL_ReadVoltages(Uint8 deviceAddr);
**/

// Initialization
Bool ISL_Init();
Bool ISL_Init_Retry(Uint8 nRetry);
Bool ISL_Init_Helper();
void ISL_Test();

// Internal callbacks
void ISL_DRDYCallback();  // used to detect incoming bytes
void ISL_TimerCallback(); // used for command timeout to figure out end of returned data or broken data // uses timer 0
void ISL_SendCommands();  // callback to send commands from the queue if there is any // uses timer 1
void ISL_ProcessPacket(); // reads a completed packet and figures out where to store it and what to do with it
Bool ISL_UpdateTable(Uint8 device, Uint8 page, Uint8* data);

// External callbacks
void ISL_SetReceiveCallback(void (*callback)());
void ISL_EnableReceiveCallback();
void ISL_DisableReceiveCallback();

// Command timer (at the mercy of the other timer) // this is really an idle timer reset by send/receive
void ISL_SetTimeout(Uint8 uS);
void ISL_SetTimeoutCallback(void (*callback)());
void ISL_StartTimeoutCallback();
void ISL_RestartTimeoutCallback();
void ISL_StopTimeoutCallback();

// Read
void ISL_ResetAvailableFlag();
Bool ISL_NewDataAvailable();
void ISL_ResetFlags();
void ISL_GuessPopulator(); // not implemented yet (reads the first byte and guesses how many bytes follow. used to quickly end sequence without wasting time waiting for it to timeout)
ISL_DEVICE* ISL_GetDevice(Uint8 deviceNumber);

// Write
Bool ISL_Write(ISL_PACKET* pack, Uint8 priority); // queues the data

// Data packing routines
void ISL_PACKET_TO_BYTES(ISL_PACKET* pack, Uint8* data);
void ISL_BYTES_TO_PACKET(Uint8* data, ISL_PACKET* pack);
void ISL_InitShadow();
void ISL_FillCRC(ISL_PACKET* pack);
void ISL_CRCHelper(Uint8* CRC, Uint8* newBit1, Uint8* newBit2, Uint8 data, Uint8 n);

// Special functions (automatic queueing)
void ISL_Reset(Uint8 device);
Bool ISL_Identify(Uint8 device, Uint8* comms);

// General read/write - asynchronous
Bool ISL_Command(Uint8 device, Uint8 page, Uint8 dAddr, Uint8 rw, Uint8* data, Uint8 dLen,Uint8 priority);
Bool ISL_ReadRegister(Uint8 device, Uint8 page, Uint8 dAddr);
Bool ISL_WriteRegister(Uint8 device, Uint8 page, Uint8 dAddr, Uint8* data);
Bool ISL_Request(Uint8 device, ISL_REQUEST req);
Uint8 NumDevices();
ISL_FLAGS* GetISLFlags();
ISL_DEVICE* GetISLDevices(Uint16 CurrentDevice);
#endif /* ISL94212_H_ */
