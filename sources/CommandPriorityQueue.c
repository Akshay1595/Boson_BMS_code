/*
 *
 * CommandPriorityQueue.c
 *
 * Author: Motivo Engineering
 * Copyright 2014	
 *
 * Summary: Fixed length and fixed number of priority queue for ISL Packet objects
 */


#include "all_header.h"

QUEUE q[CPQ_MAX_PRIORITIES];

Uint8 CPQ_NumPriorities() { return CPQ_MAX_PRIORITIES; }
Uint8 CPQ_Size() { return CPQ_MAX_PRIORITIES*CPQ_MAX_QUEUE_LENGTH; }

void CPQ_Init() {

	Uint8 i;

	for(i=0;i<CPQ_MAX_PRIORITIES;i++) {

		Queue_Init(q+i);
	}
}

/**
void CPQ_Test() {

	DisableISR();

	CPQ_Init();

	Bool flag;

	ISL_PACKET pack1;
	ISL_PACKET pack2;
	ISL_PACKET pack3;

	ISL_PACKET ret1;
	ISL_PACKET ret2;
	ISL_PACKET ret3;

	pack1.byte1 = 0x00;
	pack1.byte2 = 0x01;
	pack1.byte3 = 0x02;
	pack1.byte4 = 0x03;
	pack1.numBytes = 4;

	pack2.byte1 = 0x05;
    pack2.byte2 = 0x06;
	pack2.byte3 = 0x07;
	pack2.byte4 = 0x08;
	pack2.numBytes = 4;

	pack3.byte1 = 0x09;
	pack3.byte2 = 0x0A;
	pack3.byte3 = 0x0B;
	pack3.byte4 = 0x0C;
	pack3.numBytes = 4;

	flag = CPQ_Empty();
	flag = CPQ_Full();

	CPQ_Enqueue(&pack1);

	flag = CPQ_Empty();
	flag = CPQ_Full();

	CPQ_Enqueue(&pack2);

	flag = CPQ_Empty();
	flag = CPQ_Full();

	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();

	// should be blank, but not break queue
	ret3 = CPQ_Dequeue();

	// fill up the queue to the max
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);
	CPQ_Enqueue(&pack1);
	CPQ_Enqueue(&pack2);
	CPQ_Enqueue(&pack3);

	flag = CPQ_Empty();
	flag = CPQ_Full();

	// drain to the max
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();
	ret1 = CPQ_Dequeue();
	ret2 = CPQ_Dequeue();
	ret3 = CPQ_Dequeue();

	flag = CPQ_Empty();
	flag = CPQ_Full();

	EnableISR();
}
**/

Bool CPQ_Enqueue(ISL_PACKET* pack) {

	return CPQ_PriorityEnqueue(pack, 0);
}

Bool CPQ_PriorityEnqueue(ISL_PACKET* pack, Uint8 priority) {

	Uint8 data[ISL_COMMAND_BYTES];
	Uint8 i;
	Uint8 j;

	if(priority >= CPQ_MAX_PRIORITIES) { return False; }

	for(j=priority;j<CPQ_MAX_PRIORITIES;j++) {

		if(CPQ_PriorityFull(j) == False) {

			ISL_PACKET_TO_BYTES(pack, data);

			for(i=0;i<ISL_COMMAND_BYTES;i++) {
				Queue_Enqueue(q+j, data[i]);
			}

			return True;
		}
	}

	return False;
}

ISL_PACKET CPQ_Dequeue() {

	return CPQ_PriorityDequeue(0);
}

ISL_PACKET CPQ_PriorityDequeue(Uint8 priority) {

	Uint8 data[ISL_COMMAND_BYTES];
	Uint8 i;
	Uint8 j;

	ISL_PACKET pack;
	pack.numBytes = 0;

	if(priority >= CPQ_MAX_PRIORITIES) { return pack; }

	for(j=priority;j<CPQ_MAX_PRIORITIES;j++) {

		if(CPQ_PriorityEmpty(j) == False) {

			for(i=0;i<ISL_COMMAND_BYTES;i++) {
				data[i] = Queue_Dequeue(q+j);
			}

			ISL_BYTES_TO_PACKET(data, &pack);

			return pack;
		}
	}

	return pack;
}

ISL_PACKET CPQ_Peek() {

	Uint8 i;

	ISL_PACKET pack;
	pack.numBytes = 0;

	for(i=0;i<CPQ_MAX_PRIORITIES;i++) {

		if(CPQ_PriorityEmpty(i) == False) { return CPQ_PriorityPeek(i); }
	}

	return pack;
}

ISL_PACKET CPQ_PriorityPeek(Uint8 priority) {

	Uint8 data[ISL_COMMAND_BYTES];
	Uint8 i;

	ISL_PACKET pack;
	pack.numBytes = 0;

	if(priority >= CPQ_MAX_PRIORITIES) { return pack; }
	if(CPQ_PriorityEmpty(priority) == True) { return pack; }

	for(i=0;i<ISL_COMMAND_BYTES;i++) {
		data[i] = Queue_PeekDeep(q+priority, i);
	}

	ISL_BYTES_TO_PACKET(data, &pack);

	return pack;
}

Bool CPQ_Full() {

	Uint8 i;

	for(i=0;i<CPQ_MAX_PRIORITIES;i++) {

		if(CPQ_PriorityFull(i) == False) { return False; }
	}

	return True;
}

// if there isnt room for another command packet
Bool CPQ_PriorityFull(Uint8 priority) {

	if(priority >= CPQ_MAX_PRIORITIES) { return True; }
	if(Queue_Count(q+priority)+ISL_COMMAND_BYTES <= QUEUE_MAX_BYTES) { return False; }

	return True;
}

Bool CPQ_Empty() {

	Uint8 i;

	for(i=0;i<CPQ_MAX_PRIORITIES;i++) {

		if(CPQ_PriorityEmpty(i) == False) { return False; }
	}

	return True;
}

Bool CPQ_PriorityEmpty(Uint8 priority) {

	if(priority >= CPQ_MAX_PRIORITIES) { return False; }
	if(Queue_Count(q+priority) == 0) { return True; }

	return False;
}

void CPQ_Flush(void){
	CPQ_Init();
}


