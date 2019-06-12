/* taskFunc.c

Creates a queue to hold the location of a cell's x and y co-ordinates.

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/

#include "queue.h"

/*
	- Creates a queue that is double the size in it's arguement
	@Arg:
		queue		- cellStatus that besides the queue also holds starting and end position, plue it's capacity
		queSize		- an int that represents the size of the queue
	@Return:
	N/A
*/
void initQue (cellStatus *queue, int queSize)
{
	queue -> capacity = ((queSize * queSize) +1);
	queue -> size = 0;
	queue -> head = 0;
	queue -> tail = (queSize * queSize);
	queue -> cellQue = malloc(sizeof(cellLoc) * ((queSize * queSize) +1));;
} // end of initQue

/*
	- Checks to see if the queue is full
	@Arg:
		queue		- cellStatus that besides the queue also holds starting and end position, plue it's capacity
	@Return:
	1 if full. 0 otherwise
*/
int isFull (cellStatus *queue)
{
	return (queue -> size == queue -> capacity);
} //end of isFull

/*
	- Checks to see if the queue is empty
	@Arg:
		queue		- cellStatus that besides the queue also holds starting and end position, plue it's capacity
	@Return:
	1 if empty. 0 otherwise
*/
int isEmpty (cellStatus *queue)
{
	return (queue -> size == 0);
}// end of isEmpty

/*
	- stores a cell location in a node
	@Arg:
		row		- An int that represents the cell's row location
		col		- An int that represents the cell's column location
	@Return:
	A cellLoc struct that contains the cell row and col location
*/
static cellLoc createNode (int row, int col)
{
	cellLoc temp;

	temp.row = row;
	temp.col = col;

	return temp;
} // end of createNode

/*
	Adds the cell to the end of the queue.
	@Arg:
		queue		- cellStatus that besides the queue also holds starting and end position, plue it's capacity
		row		- An int that represents the cell's row location
		col		- An int that represents the cell's column location
	@Return:
	N/A
*/
void enqueue (cellStatus *queue, int row, int col)
{
	cellLoc temp;
	temp = createNode (row, col);

	if (isFull(queue))
		return;
	queue -> tail = (queue -> tail + 1) % queue -> capacity; // Set the tail to next end of line. If at end, wraps to front.
	queue -> cellQue [queue -> tail] = temp;	// Add node to the queue
	queue -> size += 1;
} // end of enqueue

/*
	Removes the cell from the front of the queue.
	@Arg:
		queue		- cellStatus that besides the queue also holds starting and end position, plue it's capacity
		row			- An int that represents the cell's row location
		col			- An int that represents the cell's column location
	@Return:
	N/A
*/
void dequeue (cellStatus *queue, int *row, int *col)
{
	cellLoc temp;

	if (isEmpty (queue))
		return;
	temp = queue -> cellQue [queue -> head]; // Saves the current front of the queue
	queue -> head = (queue -> head + 1) % queue -> capacity;	// Set the head to next in line. If at end, wraps to front.
	queue -> size -= 1;
	*row = temp.row; 		// Save the row location
	*col = temp.col;		// Save the column location
} // End of dequeue

/*
	Destroys the queue and frees the memory
	@Arg:
		queue		- cellStatus that besides the queue also holds starting and end position, plue it's capacity
	@Return:
		N/A
*/
void destroyQue (cellStatus *queue)
{
	free (queue -> cellQue);
	queue -> cellQue = NULL;
} // End of destroyQue
