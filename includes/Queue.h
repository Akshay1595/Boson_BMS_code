/*
 *
 * Queue.h
 *
 * Author: Jama Mohamed
 *
 * Summary: Fixed length FIFO queue with variable peek
 *
 * Modified:
 *
 * 			 Added changelist   7/1/2014 JAM
 *
 */

#ifndef QUEUE_H_
#define QUEUE_H_

#include "F28x_Project.h"     // Device Headerfile and Examples Include File

//======================= Queue Structure ===========================

#define QUEUE_MAX_BYTES 128

typedef struct {
    Uint8 arr[QUEUE_MAX_BYTES+1];  // Address of array
	Uint8 head;                    // Head pointer
	Uint8 tail;                    // Tail pointer
	Uint8 count;                   // Current number of elements
} QUEUE;

void Queue_Flush(QUEUE* q);
void Queue_Init(QUEUE* q);
Bool Queue_Enqueue(QUEUE *q, Uint8 data);
Uint8 Queue_Dequeue(QUEUE *q);
Bool Queue_Full(QUEUE *q);
Bool Queue_Empty(QUEUE *q);

inline Uint8 Queue_Peek(QUEUE *q) { return q->arr[q->head]; }
inline Uint8 Queue_PeekDeep(QUEUE *q, Uint8 offset) { return q->arr[(q->head+offset) % QUEUE_MAX_BYTES]; }
inline Uint8 Queue_Count(QUEUE *q) { return q-> count; }

#endif /* QUEUE_H_ */
