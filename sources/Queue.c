/*
 *
 * Queue.c
 *
 * Author: Motivo Engineering
 * Copyright 2014   
 *
 * Summary: Fixed length FIFO queue with variable peek
 *
 */

#include "all_header.h"


void Queue_Flush(QUEUE* q){
	while(Queue_Empty(q)==False){
		Queue_Dequeue(q);
	}
}

void Queue_Init(QUEUE* q) {

    q->head = 0;
    q->tail = QUEUE_MAX_BYTES-1;
    q->count = 0;

    return;
}

Bool Queue_Enqueue(QUEUE *q, Uint8 data) {

	if(q->count+1 > QUEUE_MAX_BYTES) { return False; }
    q->tail = (q->tail+1) % QUEUE_MAX_BYTES;
    q->arr[q->tail] = data;
    q->count = q->count+1;

    return True;
}

Uint8 Queue_Dequeue(QUEUE *q) {

	Uint8 ret;

	ret = q->arr[q->head];
    q->head = (q->head+1) % QUEUE_MAX_BYTES;
    q->count = q->count-1;

    return ret;
}

Bool Queue_Full(QUEUE *q) {

	if (q->count >= QUEUE_MAX_BYTES) { return True;	}

	return False;
}

Bool Queue_Empty(QUEUE *q) {

    if (q->count <= 0) { return True; }

    return False;
}



