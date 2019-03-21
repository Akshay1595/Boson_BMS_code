/*
 *
 * ISL94212.c
 *
 * Author: Motivo Engineering
 * Copyright 2014	
 *
 * Summary: Routines to interface with Intersil BMS via SPI
 *
 */

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "Timer.h"
#include "ISL94212.h"
#include "CommandPriorityQueue.h"
#include "SPI.h"

ISL_CALLBACKS ISLCallbacks;
ISL_STATE ISLState;
Uint8 ISLNumDevices;
Uint8 ISLCommandBuffer[4];
ISL_COMMAND_GUESS ISLCommandGuess;
ISL_FLAGS ISLFlags;
ISL_PACKET ISLIdentify;

#pragma DATA_SECTION (ISLDevices, ".isl_registers");
ISL_DEVICE ISLDevices[ISL_MAX_DEVICES];


Bool ISL_Init() {

	// Retry forever
	return ISL_Init_Retry(0);
}

Bool ISL_Init_Retry(Uint8 nRetry) {

	// If nRetry is 0, retry forever
	if(nRetry==0) {

		while(ISL_Init_Helper() == False) { DELAY_S(1); }

		return True;
	}

	while(nRetry > 0) {

		// Return if success
		if(ISL_Init_Helper() == True) { return True; }

		// Else wait and try again
		DELAY_S(1);
		nRetry--;
	}

	// Failed
	return False;
}

Bool ISL_Init_Helper() {

	Uint8 comms;

    // Set state to UNKNOWN
	// During UNKNOWN, all SPI interrupts are trashed
	ISLState = UNKNOWN;

	// Init queue
	CPQ_Init();
	//ISL_InitShadow();
	// Set SPI timeouts and callbacks
	SPI_SetTimeout(ISL_COMMAND_TIMEOUT);

	SPI_SetDRDYCallback(ISL_DRDYCallback);
	SPI_SetTimeoutCallback(ISL_TimerCallback);

	SPI_EnableDRDYCallback();
	SPI_EnableTimeoutCallback();
	ISL_DisableReceiveCallback();

	// Set COMMAND timeout and callbacks
	ISL_SetTimeout(ISL_COMMAND_PADDING);
	ISL_SetTimeoutCallback(ISL_SendCommands);

	ISL_StartTimeoutCallback();

	// Wait a second
    DELAY_S(1);

    // Send a few resets // cannot set reset befre device identified... so how does that work?
   // ISL_Reset(0x00);
    //ISL_Reset(0x01);
    //ISL_Reset(0x02);


    ISL_Request(0xF, SLEEP); // Put all the devices to sleep need this to reset them if they have been sitting for awhile
    DELAY_S(1);
    ISL_Request(0xF, WAKEUP); // Wake them all back up again
    // Wait a seconds
    DELAY_S(1);

    // Reset variables
    ISLNumDevices = 0;

    // Set state to IDENTIFY
    ISLState = IDENTIFYING;

    // Assume daisy-chain mode. Send first Identify
    // comms should be 0
    if(ISL_Identify(0, &comms) == False) { ISLState = UNKNOWN; return False; }

    // Keep sending identify until high byte of comms is 0x2
    ISLNumDevices = 0x01;
    comms = 0;
    while((comms&0x30) != 0x20) {

    	ISLNumDevices += 1;
        if(ISL_Identify(ISLNumDevices, &comms) == False) { ISLState = UNKNOWN; return False; }
    }

    // Got the last device, send final Identify
    if(ISL_Identify(0x3F, &comms) == False) { ISLState = UNKNOWN; return False; }

    // Wait a second
    DELAY_S(1);

    // Reset flags
    ISLFlags.newResponse = False;
    ISLFlags.newData = False;

    // Done initializing
    ISLState = READY;

    return True;
}


/**
void ISL_Test() {

	// Set state to UNKNOWN
	// During UNKNOWN, all SPI interrupts are trashed
	ISLState = UNKNOWN;

	// Setup timers, callbacks, and interrupts
	TMR_Init();
	SPI_Init();
	CPQ_Init();

	// Set SPI timeouts and callbacks
	SPI_SetTimeout(ISL_COMMAND_TIMEOUT);

	SPI_SetDRDYCallback(ISL_DRDYCallback);
	SPI_SetTimeoutCallback(ISL_TimerCallback);

	SPI_EnableDRDYCallback();
	SPI_EnableTimeoutCallback();

	// Set COMMAND timeout and callbacks
	ISL_SetTimeout(ISL_COMMAND_PADDING);
	ISL_SetTimeoutCallback(ISL_SendCommands);

	// Start the reoccuring timeout
	ISL_StartTimeoutCallback();
	SPI_StartTimer();


	// Send some data
	// Bogus data should elicit some response

	ISL_PACKET test;

	// fake identify
	test.byte1 = 0x03;
	test.byte2 = 0x24;
	test.byte3 = 0x00;
	test.numBytes = 3;

	// keep queueing data
	while(1) { ISL_Write(&test); }

}
**/

void ISL_DRDYCallback() {

	// receiving a packet should reset the send
	ISL_RestartTimeoutCallback();
}

// not getting another byte // either a problem with comms or xfer completed
void ISL_TimerCallback() {

	// Timer was already stopped. we just need to process the data
	ISL_ProcessPacket();

	// Process the callback
	if(ISLCallbacks.enable[0] == True) { (*(ISLCallbacks.callback[0]))(); }
}

// callback to send commands from the queue if there is any // uses timer 1
void ISL_SendCommands() {

	//SPI_StopTimer();


	// if the queue is not empty...
	if(CPQ_Empty() == False) {

		// pull the highest priority packet out
		ISL_PACKET pack = CPQ_Dequeue();

		// make sure it is the correct length
		if(pack.numBytes == 3 || pack.numBytes == 4) {

			// send the bytes
			DELAY_US(ISL_BYTE_PADDING);
			SPI_Write(&(pack.byte1));
			DELAY_US(ISL_BYTE_PADDING);
			SPI_Write(&(pack.byte2));
			DELAY_US(ISL_BYTE_PADDING);
			SPI_Write(&(pack.byte3));
			DELAY_US(ISL_BYTE_PADDING);

            if(pack.numBytes == 4) { SPI_Write(&(pack.byte4)); DELAY_US(ISL_BYTE_PADDING); }
		}
	}

	// sending a packet should reset the send
	ISL_RestartTimeoutCallback();
}

// reads a completed packet and figures out where to store it and what to do with it
void ISL_ProcessPacket() {

	Uint8 device;
	Uint16 header;

	// Grab the data
	SPI_RECEIVE_BUFFER* ISL_ReceiveBuffer = SPI_GetReadBuffer();

	// If there are too few bytes, don't bother. just set timeout flag
	if(ISL_ReceiveBuffer->numBytes < 3) { ISLFlags.timeout = True; return; }

	// Grab header (page/addr)
	device = ((ISL_ReceiveBuffer->data[0])&0xF0)>>4;
	header = ((((Uint16)((ISL_ReceiveBuffer->data[0])&0x07))<<6) | (((Uint16)((ISL_ReceiveBuffer->data[1])&0xFC))>>2));

	//Set the header into the flags
	ISLFlags.Header=header;

	// devices must make sense
	if(device > ISL_MAX_DEVICES) { return; }

	// ========================= Management =========================
	// Special case: check to see if it is an identify
	if(header == IDENTIFY) {

		// must be exactly 4 bytes
		if(ISL_ReceiveBuffer->numBytes != 4) { ISLFlags.timeout = True; return; }

		// store packet into identify packet
		ISLIdentify.byte1 = ISL_ReceiveBuffer->data[0];
		ISLIdentify.byte2 = ISL_ReceiveBuffer->data[1];
		ISLIdentify.byte3 = ISL_ReceiveBuffer->data[2];
		ISLIdentify.byte4 = ISL_ReceiveBuffer->data[3];
		ISLIdentify.numBytes = 4;

		ISLFlags.newIdentify = True;

		if(ISL_ReceiveBuffer->numBytes > 0) { ISLFlags.newData = True; }
		if(ISL_ReceiveBuffer->numBytes > 0) { ISLFlags.newResponse = True; }

		return;
	}

	// Special case: ACK
	if(header == ACK) {

		ISLFlags.Device = device;
		ISLFlags.newAck = True;

		if(ISL_ReceiveBuffer->numBytes > 0) { ISLFlags.newData = True; }
		if(ISL_ReceiveBuffer->numBytes > 0) { ISLFlags.newResponse = True; }

		return;
	}

	// Special case: NAK
	if(header == NAK) {

	    ISLFlags.nak = True;
		ISLFlags.Device = device;
		return;
	}


	// ========================= Burst Transfers =========================
	// ========================= Normal Transfers =========================

	// Both of these cases can be handled at the same time! lucky for us, smaller code size!
	// It could be the response to a burst command. Has the format 4bytes - 3bytes - 3bytes --//--> 3bytes - END
	// Or a regular command                                        4bytes - END
	// In either case, the remainder must be mod3
	if(ISL_ReceiveBuffer->numBytes >= 4 && ((ISL_ReceiveBuffer->numBytes-4)%3==0)) {

		Uint8 i;
		Uint8 numValues = ((ISL_ReceiveBuffer->numBytes-4)/3)+1;

		// Is it in Page 1?
		if((ISL_PAGE1_START_MASK&header) == ISL_PAGE1_START_ADDR) {

			// We know it is a page 1 command, so group into bytes and load into table
			for(i=0;i<numValues;i++) {

				if(ISL_UpdateTable(device-1, 0, ISL_ReceiveBuffer->data+(1+i*3)) == False) { ISLFlags.tableError = True; }
			}

			ISLFlags.newData = True;
			ISLFlags.Device = device;
		}

		// Is it in Page 2_1?
		if((ISL_PAGE2_1_START_MASK&header) == ISL_PAGE2_1_START_ADDR) {

			// We know it is a page 2_1 command, so group into bytes and load into table
			for(i=0;i<numValues;i++) {

				if(ISL_UpdateTable(device-1, 1, ISL_ReceiveBuffer->data+(1+i*3)) == False) { ISLFlags.tableError = True; }
			}

			ISLFlags.newData = True;
			ISLFlags.Device = device;
		}

		// Is it in Page 2_2?
		if((ISL_PAGE2_2_START_MASK&header) == ISL_PAGE2_2_START_ADDR) {

			// We know it is a page 2_2 command, so group into bytes and load into table
			for(i=0;i<numValues;i++) {

				if(ISL_UpdateTable(device-1, 2, ISL_ReceiveBuffer->data+(1+i*3)) == False) { ISLFlags.tableError = True; }
			}

			ISLFlags.newData = True;
			ISLFlags.Device = device;
		}

		// Is it in Page 2_3?
		if((ISL_PAGE2_3_START_MASK&header) == ISL_PAGE2_3_START_ADDR) {

			// We know it is a page 2_3 command, so group into bytes and load into table
			for(i=0;i<numValues;i++) {

				if(ISL_UpdateTable(device-1, 3, ISL_ReceiveBuffer->data+(1+i*3)) == False)  { ISLFlags.tableError = True; }
			}

			ISLFlags.newData = True;
			ISLFlags.Device = device;
		}
	}


	/**
	// ========================= Normal Transfers =========================

	// Anything that is not special is treated as a normal command
	if(ISL_ReceiveBuffer->numBytes == 4) {

		// Is it in Page 1?
		if((ISL_PAGE1_START_MASK&header) == ISL_PAGE1_START_ADDR) {

			// We know it is a page 1 normal command, so load the data
			ISL_UpdateTable(device-1, 0, ISL_ReceiveBuffer->data+1);

			ISLFlags.newData = True;
		}

		// Is it in Page 2_1?
		if((ISL_PAGE2_1_START_MASK&header) == ISL_PAGE2_1_START_ADDR) {

			// We know it is a page 1 normal command, so load the data
			ISL_UpdateTable(device-1, 1, ISL_ReceiveBuffer->data+1);

			ISLFlags.newData = True;
		}

		// Is it in Page 2_2?
		if((ISL_PAGE2_2_START_MASK&header) == ISL_PAGE2_2_START_ADDR) {

			// We know it is a page 1 normal command, so load the data
			ISL_UpdateTable(device-1, 2, ISL_ReceiveBuffer->data+1);

			ISLFlags.newData = True;
		}

		// Is it in Page 2_3?
		if((ISL_PAGE2_3_START_MASK&header) == ISL_PAGE2_3_START_ADDR) {

			// We know it is a page 1 normal command, so load the data
			ISL_UpdateTable(device-1, 3, ISL_ReceiveBuffer->data+1);

			ISLFlags.newData = True;
		}
	}
	**/

	if(ISL_ReceiveBuffer->numBytes > 0) { ISLFlags.newResponse = True; }
}

// Always expects 3 bytes raw, page picks .PAGExx
Bool ISL_UpdateTable(Uint8 device, Uint8 page, Uint8* data) {

	Uint8  dAddr;
	Uint16 cData;

	dAddr = ((data[0])>>2)&0x3F;
	cData = ((((Uint16)(data[0]))<<12)&0x3000) | ((((Uint16)(data[1]))<<4)&0x0FF0) | ((((Uint16)(data[2]))>>4)&0x000F);

	// Memory overflow protection
	if(page==0) { if(dAddr >= ISL_PAGE1_LENGTH)   { return False; } *(((Uint16*)(&(ISLDevices[device].PAGE1)))+(dAddr&ISL_PAGE1_OFFSET_MASK))   = cData; }
	if(page==1) { if(dAddr >= ISL_PAGE2_1_LENGTH+ISL_PAGE2_2_LENGTH+ISL_PAGE2_3_LENGTH) { return False; } *(((Uint16*)(&(ISLDevices[device].PAGE2_1)))+(dAddr&ISL_PAGE2_1_OFFSET_MASK)) = cData; }
	if(page==2) { if(dAddr >= ISL_PAGE2_1_LENGTH+ISL_PAGE2_2_LENGTH+ISL_PAGE2_3_LENGTH) { return False; } *(((Uint16*)(&(ISLDevices[device].PAGE2_2)))+(dAddr&ISL_PAGE2_2_OFFSET_MASK)) = cData; }
	if(page==3) { if(dAddr >= ISL_PAGE2_1_LENGTH+ISL_PAGE2_2_LENGTH+ISL_PAGE2_3_LENGTH) { return False; } *(((Uint16*)(&(ISLDevices[device].PAGE2_3)))+(dAddr&ISL_PAGE2_3_OFFSET_MASK)) = cData; }

	return True;
}

// Command timer (at the mercy of the other timer) // this is really an idle timer reset by send/receive
void ISL_SetTimeout(Uint8 uS) {

	TMR_SetTimeout(GNR_ISL_TIMER, uS);
}

void ISL_SetTimeoutCallback(void (*callback)()) {

	TMR_SetTimeoutCallback(GNR_ISL_TIMER, callback);
}

void ISL_StartTimeoutCallback() {

	TMR_StartTimer(GNR_ISL_TIMER);
}

void ISL_RestartTimeoutCallback() {

	TMR_ResetTimer(GNR_ISL_TIMER);
}

void ISL_StopTimeoutCallback() {

	TMR_StopTimer(GNR_ISL_TIMER);
}

void ISL_EnableReceiveCallback() {

	ISLCallbacks.enable[0] = True;
}

void ISL_DisableReceiveCallback() {

	ISLCallbacks.enable[0] = False;
}

void ISL_SetReceiveCallback(void (*callback)()) {

	ISLCallbacks.callback[0] = callback;
}

// Read
void ISL_ResetAvailableFlag() {

	ISLFlags.newData = False;
}

void ISL_ResetFlags() {

	// Reset flags
	ISLFlags.newIdentify = False;
	ISLFlags.timeout = False;
	ISLFlags.newAck = False;
	ISLFlags.newData = False;
	ISLFlags.nak = False;
	ISLFlags.newResponse = False;
	ISLFlags.Device=0x0000;
	ISLFlags.Header=0x0000;
}

Bool ISL_NewDataAvailable() {

	return ISLFlags.newData;
}


ISL_DEVICE* ISL_GetDevice(Uint8 deviceNumber) {

	// default to first device
	if(deviceNumber >= ISL_MAX_DEVICES) { return ISLDevices; }

	return ISLDevices+deviceNumber;
}

// Write
// queues the data // lowest priority
Bool ISL_Write(ISL_PACKET* pack,Uint8 priority) {

	return CPQ_PriorityEnqueue(pack, priority);
}

// Data packing routines
void ISL_PACKET_TO_BYTES(ISL_PACKET* pack, Uint8* data) {

	Uint8 i;

	for(i=0;i<ISL_COMMAND_BYTES;i++) {

		data[i] = ((Uint8*)pack)[i];
	}
}

void ISL_BYTES_TO_PACKET(Uint8* data, ISL_PACKET* pack) {

	Uint8 i;

	for(i=0;i<ISL_COMMAND_BYTES;i++) {

		((Uint8*)pack)[i] = data[i];
	}
}

void ISL_InitShadow() {

	Uint8 i;
	Uint16 j;

	for(i=0;i<ISL_MAX_DEVICES;i++) {

		for(j=0;j<sizeof(ISL_DEVICE);j++) {

			*(((Uint16*)&(ISLDevices[i]))+(j)) = 0x0000;
		}
	}
}

void ISL_FillCRC(ISL_PACKET* pack) {

	// Assume we want to CRC 2.5 bytes
	Uint8 CRC = 0;
	Uint8 newBit1, newBit2;

	Uint8 byte1 = pack->byte1;
	Uint8 byte2 = pack->byte2;
	Uint8 byte3 = pack->byte3;
	Uint8 byte4 = pack->byte4;

	// CRC the first byte
	ISL_CRCHelper(&CRC, &newBit1, &newBit2, byte1, 7);

	// CRC the second byte
	ISL_CRCHelper(&CRC, &newBit1, &newBit2, byte2, 7);

	// three byte closing
	if(pack->numBytes == 3) {

		// CRC the third half byte
		ISL_CRCHelper(&CRC, &newBit1, &newBit2, byte3, 3);

		// Store calculated CRC in message
		pack->byte3 |= CRC;
	}

	// four byte closing
	if(pack->numBytes == 4) {

		// CRC the third byte
		ISL_CRCHelper(&CRC, &newBit1, &newBit2, byte3, 7);

		// CRC the fourth half byte
		ISL_CRCHelper(&CRC, &newBit1, &newBit2, byte4, 3);

		// Store calculated CRC in message
		pack->byte4 |= CRC;
	}

	return;
}

void ISL_CRCHelper(Uint8* CRC, Uint8* newBit1, Uint8* newBit2, Uint8 data, Uint8 n) {

	Uint8 i;

	// run CRC on the byte
	for(i=0;i<=n;i++) {
		*newBit1 = ((*CRC>>3)&0x1)^((data>>7)&0x1);
		*newBit2 = ((*CRC>>3)&0x1)^((*CRC>>0)&0x1);
		*CRC = *CRC << 1; data = data << 1;
		*CRC = (*CRC&0xC)|(*newBit2<<1)|(*newBit1<<0);
	}
}

// Special functions (automatic queueing)
void ISL_Reset(Uint8 device) {

	ISL_PACKET pack;

	// Send a reset
	pack.byte1 = 0x03;
	pack.byte2 = 0x48;
    pack.byte3 = 0x0c;

    // Insert device number
    pack.byte1 |= (device<<4);

    // Calculate CRC
    ISL_FillCRC(&pack);

	// Set length
	pack.numBytes = 3;

	// Reset flags
	ISLFlags.timeout = False;

	// Queue command
	ISL_Write(&pack, 0);

	// Wait for timeout
	while(ISLFlags.newResponse == False && ISLFlags.timeout == False) { }
}

Bool ISL_Identify(Uint8 device, Uint8* comms) {

    ISL_PACKET pack;

    // Send an identify
    pack.byte1 = 0x03;
    pack.byte2 = 0x24;
    pack.byte3 = 0x00;

    // Insert device number
    pack.byte2 |= (device>>4);
    pack.byte3 |= ((device<<4)&0xF0);

    // Set length
    pack.numBytes = 3;

    // Calculate CRC
    ISL_FillCRC(&pack);

    // Reset flags
    ISL_ResetFlags();

    // Queue command
    ISL_Write(&pack, 0);

    // Wait for data return
    while(ISLFlags.newResponse == False && ISLFlags.timeout == False && ISLFlags.nak == False) { }

    //if special_case nak i.e. no acknowledgement
    if(ISLFlags.nak == True){ return False;}

    // If timeout, return false
    if(ISLFlags.timeout == True) { return False; }

	// If device 0 was probed, we are expecting an ACK
	if(device == 0 && ISLFlags.newAck == False) { return False; }

	// If device 0x3F was probed, we are expecting an ACK
	if(device == 0x3F && ISLFlags.newAck == False) { return False; }

	// Else, we are expecting an identify
	if(device != 0x3F && device != 0 && ISLFlags.newIdentify == False) { return False; }

    // Pull out comms state and device number (may not be valid if ACK was returned)
    *comms = ((ISLIdentify.byte2&0x3)<<4)|(ISLIdentify.byte3&0xF);

    return True;
}


Bool ISL_Command(Uint8 device, Uint8 page, Uint8 dAddr, Uint8 rw, Uint8* data, Uint8 dLen, Uint8 priority) {

    ISL_PACKET pack;

    // Pack the data
    pack.byte1 = ((device<<4)&0xF0) | ((rw<<3)&0x08) | (page&0x3);
    pack.byte2 = ((dAddr<<2)&0xFC);
    pack.byte3 = 0x00;
    pack.byte4 = 0x00;

    // Set length
    pack.numBytes = 3;

    if(dLen==1) {

    	pack.byte2 |= ((data[0])>>4)&0x03;
    	pack.byte3 |= ((data[0])<<4)&0xF0;
    }

    if(dLen==2) {

    	pack.byte2 |= ((data[0])>>4)&0x03;
    	pack.byte3 |= (((data[0])<<4)&0xF0) | (((data[1])>>4)&0x0F);
    	pack.byte4 |= ((data[1])<<4)&0xF0;

    	// Set length
    	pack.numBytes = 4;
    }

    // Calculate CRC
    ISL_FillCRC(&pack);

    // Reset flags
    ISL_ResetFlags();

    // Queue command
    return ISL_Write(&pack, priority);
}

Bool ISL_ReadRegister(Uint8 device, Uint8 page, Uint8 dAddr) {

	return ISL_Command(device, page, dAddr, 0, 0, 0, 1);
}

Bool ISL_WriteRegister(Uint8 device, Uint8 page, Uint8 dAddr, Uint8* data) {

	return ISL_Command(device, page, dAddr, 1, data, 2, 0);
}

Bool ISL_Request(Uint8 device, ISL_REQUEST req) {

	return ISL_Command(device, (req>>6)&0x3, req&0x3F, 0, 0, 0, 1);
}

Uint8 NumDevices(){
	return ISLNumDevices;
}

ISL_FLAGS* GetISLFlags() {
	return &ISLFlags;
}

ISL_DEVICE* GetISLDevices(Uint16 CurrentDevice) {
	return ISLDevices+CurrentDevice;
}
