/*
 *
 * CommandPriorityQueue.h
 *
 * Author: Jama Mohamed
 *
 * Summary: Fixed length and fixed number of priority queue for ISL Packet objects
 *
 * Modified:
 *
 * 			 Added changelist   7/1/2014 JAM
 *
 */

#ifndef COMMANDPRIORITYQUEUE_H_
#define COMMANDPRIORITYQUEUE_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File
#include "General.h"
#include "ISL94212.h"

#define CPQ_MAX_PRIORITIES 3
#define CPQ_MAX_QUEUE_LENGTH (QUEUE_MAX_BYTES/ISL_COMMAND_BYTES)

void CPQ_Init();
void CPQ_Test();
Bool CPQ_Enqueue(ISL_PACKET* data);
Bool CPQ_PriorityEnqueue(ISL_PACKET* data, Uint8 priority);
ISL_PACKET CPQ_Dequeue();
ISL_PACKET CPQ_PriorityDequeue(Uint8 priority);
ISL_PACKET CPQ_Peek();
ISL_PACKET CPQ_PriorityPeek(Uint8 priority);
Bool CPQ_Full();
Bool CPQ_PriorityFull(Uint8 priority); // if there isnt room for another command packet
Bool CPQ_Empty();
Bool CPQ_PriorityEmpty(Uint8 priority);
void CPQ_Flush(void);

#endif /* COMMANDPRIORITYQUEUE_H_ */
