/* taskFunc.c

Creates a queue to hold the location of a cell's x and y co-ordinates.

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/

#include <stdlib.h>

#ifndef _QUEUE_H_
#define _QUEUE_H_

typedef struct cell
{
	int row;
	int col;
}cellLoc;

typedef struct queue
{
	int capacity; 		// capacity of queue
	int size;			// current size of queue
	int head;			// front of queue
	int tail;			// rear of queue
	cellLoc * cellQue;
} cellStatus;

void initQue (cellStatus *queue, int queSize);

int isFull (cellStatus *queue);

int isEmpty (cellStatus *queue);

void enqueue (cellStatus *queue, int row, int col);

void dequeue (cellStatus *queue, int *row, int *col);

void destroyQue (cellStatus *queue);

#endif
