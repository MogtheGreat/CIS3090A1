/* taskFunc.c

Contains set of functions to assign roles to threads and control them.

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/

#include "taskFunc.h"

/*
	- Checks to see if the command line arguements are the right
	type
	@Arg:
	argc		- an int that represents the number of arguements on the command line
	argv		- an array of strings that represents the command line arguments
	@Return:
	0 if something is wrong
	1 if all correct
*/
static int validateArg (int argc, char * argv[])
{
	int i;
	int temp = -1;

	// Checks the first 2 arguments to see if they are ints
	for (i = 1; i < 3; i++)
	{
		temp = atoi(argv[1]);
		if (temp <= -1)
		{
			return 0;
		}
		temp = -1;
	}

	// Checks the last argument to see if it is -d
	if ((argc == 4) && (strcmp(argv[3], "-d") != 0))
	{
		return 0;
	}

	return 1;
} // End of validateArg

/*
	- Goes through the command line arguement to make sure that it has arguments, that
	the arguemnts are the right type and that they are the correct value.
	@Arg:
	argc		- an int that represents the number of arguements on the command line
	argv		- an array of strings that represents the command line arguments
	@Return:
	0 if something is wrong
	1 if all correct
*/
int checkArg (int argc, char * argv[])
{
	int check = 0;

	// Not counting the executable as an arguement
	// Less then 2 arguements, the bare min
	if (argc < 3)
	{
		printf ("There must be at least 2 arguments in the command line!\n");
		printf ("The required arguments are grid-size and the number of iterations.\n");
		return 0;
	}

	// More then 3 arguements, the most it should accept
	if (argc > 4)
	{
		printf ("There is too many arguments in the command line!\n");
		printf ("Please remove %d arguements.\n", (argc - 4));
		return 0;
	}

	// Checks to see if the arguements are the right type
	if (!(validateArg(argc, argv)))
	{
		printf ("Incorrect command line arguements!\n");
		printf ("Acceptable command line arguements: ./data # # or ./data # # -d\n");
		return 0;
	}

	// Checks to see if the value of the first 2 arguements are the right value
	check = atoi(argv[1]);

	if (check <= 0)
	{
		printf ("Value for grid size must be greater than 0.");
		return 0;
	}

	check = atoi(argv[2]);

	if (check <= 0)
	{
		printf ("Value for number of iterations must be greater than 0.");
		return 0;
	}

	return 1;
} // end of checkArg

/*
	- Creates a board to the size indicated in the command line
	@Arg: N/A
	@Return:
	Returns 1 if both boards were created. 0 if out of memory.
*/
static int createGrid ()
{
	int i;

	gridOne = malloc (sizeof(int*) * gridSize);
	gridTwo = malloc (sizeof(int*) * gridSize);

	// Checks to see if mallocing for the rows has work
	if ((gridOne == NULL) && (gridTwo == NULL))
		return 0;

	for (i = 0; i < gridSize; i++)
	{
		gridOne[i] = malloc (sizeof(int) * gridSize);
		gridTwo[i] = malloc (sizeof(int) * gridSize);

		// Checks to see if mallocing for the columns has work
		if ((gridOne[i] == NULL) && (gridTwo[i] == NULL))
			return 0;
	}

	return 1;
} // End of createGrid ()

/*
	- Initialize all global variables within the program
	@Arg:
	argv		- an array of strings that represents the command line arguments
	@Return:
	1 if all global variables are initialized. 0 if the mutexes and grids are 
	having trouble.
*/
int initGlobal (char *argv[])
{
	gridSize = atoi(argv[1]);
	curGrid = 0;
	countThr = 0;
	done = 0;
	initQue(&addQue, gridSize);
	initQue(&removeQue, gridSize);
	pthread_cond_init(&allToGet, NULL);

	// Initializes the mutexes and condition variable
	if (pthread_mutex_init(&countMutex, NULL) != 0)
	{
		return 	0;
	}

	if (pthread_mutex_init(&addMutex, NULL) != 0)
	{
		return 	0;
	}

	if (pthread_mutex_init(&removeMutex, NULL) != 0)
	{
		return 	0;
	}

	// Create the grids for the game
	if (!(createGrid()))
	{
		return 0;
	}

	return 1;
} // End of initGlobal 

/*
	- Fills both boards with zeros
	@Arg:
	N/A
	@Return:
	N/A
*/
void fillGrid ()
{
	int i;
	int j;

	for (i = 0; i < gridSize; i++)
	{
		for (j = 0; j < gridSize; j++)
		{
			gridOne[i][j] = 0;
			gridTwo[i][j] = 0;
		}
	}
} // End of fillGrids

/*
	- Generates a small number of living cells on the first board, using a random generator
	@Arg:
	N/A
	@Return:
	N/A
*/
void createStart ()
{
	time_t t;
	int x = 0;
	int y = 0;
	int numLife = (gridSize / 2) * (gridSize / 2); // Divides size by half then multiplies it by the power of two

	srand((unsigned) time(&t));	// Initialize the random generator using time

	while (numLife != 0)
	{
		// Choose a random spot on the grid
		x = rand () % gridSize;
		y = rand () % gridSize;

		// If the spot on the grid is dead, make it alive
		if (gridOne[x][y] == 0)
		{
			gridOne[x][y] = 1;
			numLife -= 1;
		} 
	}
} // End of createStart

/*
	- A barrier that holds the threads until all are there.
	@Arg:
	rank 	- a long int that represents the thread's rank. Originally meant for debugging purposes.
	@Return:
	N/A
	Note:
	- Only works first time. Does not work after first round. Problem with 2nd or 3rd thred.
*/
static void* holdUntilReady (long rank)
{
	// Locks the first counting mutex
	pthread_mutex_lock(&countMutex);
	countThr += 1;

	// If last thread, reset count and boolean. Wake up all threads.
	if (countThr == 3)
	{
		countThr = 0;
		done = 0;
		pthread_cond_broadcast(&allToGet);
	}
	// Not last thread
	else
	{
		while (pthread_cond_wait(&allToGet, &countMutex) != 0);
	}
	pthread_mutex_unlock(&countMutex);

	return NULL;
} // end of holdUntilReady

/*
	- Assign roles to the threads based on rank
	@Arg:
	rank 		- void pointer that contains the thread's rank (long int)
	@Return:
	NULL
*/
void* startTasks (void* rank)
{
	long myRank = (long) rank;
	holdUntilReady (myRank);		// Holds all the threads until all are created

	switch (myRank)
	{
		case 0	:	checkGrid (myRank);			// Goes through a grid and calculates whatever a cell lives or dies
					break;
		case 1	:	manageAdd (myRank);			// Goes through a queue and assign 'live' value to a cell on a new board
					break;
		case 2	:	manageRemove (myRank);		// Goes through a queue and assign 'dead' value to a cell on a new board
					break;
	}

	return NULL;
} // end of startTasks

/*
	-checks a cell's location to see what neighbours it has
	@Arg:
	row 	- an int that represent a row on the grid
	col 	- an int that represent a column on the grid
	@Return:
	- An int that represents one of eight spots on the grid
	0 		- Top of the grid
	1 		- Bottom of the grid
	2 		- Left side of the grid
	3 		- Right side of the grid
	4 		- Upper Left cornor
	5		- Upper Right cornor
	6 		- Bottom Left cornor
	7 		- Bottom Right cornor
	8		- anywhere else
*/
static int nearBoundry (int row, int column)
{
	int end = gridSize - 1;

	// Top of the grid has  been found, exclude corners
	if ((row == 0) && !((column == 0) || (column == end)))
		return 0;

	// Bottom of the grid has been found, exclude corners
	else if ((row == end) && !((column == 0) || (column == end)))
		return 1; 

	// Left side of the grid has been found, exlude top and bottom
	else if ((column == 0) && !((row == 0) || (row == end)))
		return 2;

	// Right side of the grid has been found, exlude top and bottom
	else if ((column == end) && !((row == 0) || (row == end)))
		return 3;

	// Upper Left cornor of the grid
	if ((row == 0) && (column == 0))
		return 4;

	// Upper Right cornor of the grid
	else if ((row == 0) && (column == end))
		return 5;

	// Bottom Left cornor of the grid
	else if ((row == end) && (column == 0))
		return 6;

	// Bottom Right cornor of the grid
	else if ((row == end) && (column == end))
		return 7;

	// Center part of the grid
	else
		return 8;
} // End of nearBoundry

/*
	- Checks the cell's neighbour's status. Result determines whatever if the cell
	is alive or dead.
	@Arg:
		row 		- An int that represents the row in the grid
		col 		- An int that represents the column in the grid
		live		- An int that represents the condition of the cell
	@Return:
	1 - cell is alive
	0 - cell is dead
*/
static int checkNeighbours (int row, int col, int live)
{
	int check;
	int total;

	check = nearBoundry (row, col);	// Checks to see if cell near the border of the grid
	total = 0;

	switch (check)
	{
		// Checks the neighbours around the top row, excluding corners (left to down to right)
		case 0	:	if (curGrid == 0)
					{
						total = gridOne[row][(col-1)] + gridOne[(row+1)][(col-1)] + gridOne[(row+1)][col] + gridOne[(row+1)][(col+1)] + gridOne[row][(col+1)];
					}
					else
					{
						total = gridTwo[row][(col-1)] + gridTwo[(row+1)][(col-1)] + gridTwo[(row+1)][col] + gridTwo[(row+1)][(col+1)] + gridTwo[row][(col+1)];
					}
					break;

		// Checks the neighbours around the bottom row, excluding corners (left to up to right)
		case 1	:	if (curGrid == 0)
					{
						total = gridOne[row][(col-1)] + gridOne[(row-1)][(col-1)] + gridOne[(row-1)][col] + gridOne[(row-1)][(col+1)] + gridOne[row][(col+1)];
					}
					else
					{
						total = gridTwo[row][(col-1)] + gridTwo[(row-1)][(col-1)] + gridTwo[(row-1)][col] + gridTwo[(row-1)][(col+1)] + gridTwo[row][(col+1)];
					}
					break;
		// Cecks the neighbours around the left side of the grid (top to rigt to bottom.)
		case 2	:	if (curGrid == 0)
					{
						total = gridOne[(row-1)][(col)] + gridOne[(row-1)][(col+1)] + gridOne[row][(col+1)] + gridOne[(row+1)][(col+1)] + gridOne[(row+1)][col];
					}
					else
					{
						total = gridTwo[(row-1)][(col)] + gridTwo[(row-1)][(col+1)] + gridTwo[row][(col+1)] + gridTwo[(row+1)][(col+1)] + gridTwo[(row+1)][col];
					}
					break;
		// Cecks the neighbours around the right side (top to left to bottom)
		case 3	:	if (curGrid == 0)
					{
						total = gridOne[(row-1)][col] + gridOne[(row-1)][(col-1)] + gridOne[row][(col-1)] + gridOne[(row+1)][(col-1)] + gridOne[(row+1)][col];
					}
					else
					{
						total = gridTwo[(row-1)][col] + gridTwo[(row-1)][(col-1)] + gridTwo[row][(col-1)] + gridTwo[(row+1)][(col-1)] + gridTwo[(row+1)][col];
					}
					break;
		// Cecks the neighbours around the upper left cornor (right to bottom)
		case 4	:	if (curGrid == 0)
					{
						total = gridOne[row][(col+1)] + gridOne[(row+1)][(col+1)] + gridOne[(row+1)][col];
					}
					else
					{
						total = gridTwo[row][(col+1)] + gridTwo[(row+1)][(col+1)] + gridTwo[(row+1)][col];
					}
					break;
		// Cecks the neighbours around the upper right cornor (left to bottom)
		case 5	:	if (curGrid == 0)
					{
						total = gridOne[row][(col-1)] + gridOne[(row+1)][(col-1)] + gridOne[(row+1)][col];
					}
					else
					{
						total = gridTwo[row][(col-1)] + gridTwo[(row+1)][(col-1)] + gridTwo[(row+1)][col];
					}
					break;
		// Cecks the neighbours around the bottom left cornor (right to top)
		case 6	:	if (curGrid == 0)
					{
						total = gridOne[(row-1)][col] + gridOne[(row-1)][(col+1)] + gridOne[row][(col+1)];
					}
					else
					{
						total = gridTwo[(row-1)][col] + gridTwo[(row-1)][(col+1)] + gridTwo[row][(col+1)];
					}
					break;
		// Cecks the neighbours around the bottom right cornor (left to top) 
		case 7	:	if (curGrid == 0)
					{
						total = gridOne[(row-1)][col] + gridOne[(row-1)][(col-1)] + gridOne[row][(col-1)];
					}
					else
					{
						total = gridTwo[(row-1)][col] + gridTwo[(row-1)][(col-1)] + gridTwo[row][(col-1)];
					}
					break;
		// Cecks the neighbours around cell
		default	:
				if (curGrid == 0)
					{
						total = gridOne[(row-1)][col] + gridOne[(row-1)][(col+1)] + gridOne[row][(col+1)] + gridOne[(row+1)][(col+1)] + gridOne[(row+1)][col] + gridOne[(row+1)][(col-1)] + gridOne[row][col-1] + gridOne[(row-1)][(col-1)];
					}
					else
					{
						total = gridTwo[(row-1)][col] + gridTwo[(row-1)][(col+1)] + gridTwo[row][(col+1)] + gridTwo[(row+1)][(col+1)] + gridTwo[(row+1)][col] + gridTwo[(row+1)][(col-1)] + gridTwo[row][col-1] + gridTwo[(row-1)][(col-1)];
					}
	} // end of switch

	// Less then two alive neighbours or more than three
	if ((total < 2) || (total > 3))
	{
		return 0;
	}
	// Exactly two alive neighbours and cell itself is alive
	else if ((total == 2) && (live == 1))
	{
		return 1;
	}
	// Exactly three alive neighbours. Cell is alive regardless of it initial status
	else if (total == 3)
	{
		return 1;
	}
	// Dead Cell (THIS CAN'T BE! I AM PERFECTION! ..... Sorry DBz joke)
	else
	{
		return 0;
	}
} // End of checkNeighbours

/*
	- Shows all the cell's status in the current grid
	@Arg:
	N/A
	@Return:
	N/A
*/
void displayGrid ()
{
	int i;
	int j;

	if (DEBUG)
	{
		if (curGrid == 0)
		{
			printf ("Displaying Grid Two.\n");
		}
		else
		{
			printf ("Displaying Grid One.\n");
		}
	}

	for (i = 0; i < gridSize; i++)
	{
		for (j = 0; j < gridSize; j++)
		{
			if (curGrid == 0)
			{
				printf ("%d", gridTwo[i][j]);
			}
			else
			{
				printf ("%d", gridOne[i][j]);
			}
		}
		printf("\n");
	}
	printf("\n");
} // End of displayGrid

/*
	- Goes through each cell in current grid and checks it's neighbours. If meets requirements, cell is
	alive. Otherwise dead. Border of grid treated as dead cells.
	@Arg:
	myRank		- a long int that represents the thread's rank. Originally meant for debugging purposes.
	@Return:
	N/A
*/
void checkGrid (long myRank)
{
	int i;
	int j;
	int temp = 0;

	for (i = 0; i < gridSize; i++)
	{
		for (j = 0; j < gridSize; j++)
		{
			// Checks to see if cell is alive base on it's neighbours
			if (curGrid == 0)
			{
				temp = checkNeighbours (i, j, gridOne[i][j]);
			}
			else
			{
				temp = checkNeighbours (i, j, gridTwo[i][j]);
			}

			// Adds cell to add queue or remove base on it's status
			if (temp == 1)
			{
				pthread_mutex_lock(&addMutex);
				enqueue (&addQue, i, j);
				pthread_mutex_unlock(&addMutex);
			}
			else
			{
				pthread_mutex_lock(&removeMutex);
				enqueue (&removeQue, i, j);
				pthread_mutex_unlock(&removeMutex);
			}

			if (DEBUG3)
				printf  ("Thread %ld has gone insane\n", myRank);
		}// end of inner for loops
	}

	done = 1;

	return;
} // end of checkGrid

/*
	- Will wait for first thread to be done before closing. Dequeue
	and adds it to the cell. Hold alive cells
	@Arg:
	myRank		- a long int that represents the thread's rank. Originally meant for debugging purposes.
	@Return:
	N/A
*/
void manageAdd (long myRank)
{
	int row = 0;
	int col = 0;

	// First thread not done and add queue is not empty
	while ((done != 1) || !(isEmpty (&addQue)))
	{	
		// There is something in the add queue
		if (!(isEmpty (&addQue)))
		{
			// remove entry from queue
			pthread_mutex_lock(&addMutex);
			dequeue (&addQue, &row, &col);
			pthread_mutex_unlock(&addMutex);

			// Add alive cell to new board
			if (curGrid == 0)
			{
				gridTwo[row][col] = 1;
			}
			else
			{
				gridOne[row][col] = 1;
			}

		}

		if (DEBUG3)
			printf  ("Thread %ld has gone insane\n", myRank);
	} // end of outer while loop
} // End of manageAdd

/*
	- Will wait for first thread to be done before closing. Dequeue
	and adds it to the cell. Holds dead cells.
	@Arg:
	myRank		- a long int that represents the thread's rank. Originally meant for debugging purposes.
	@Return:
	N/A
*/
void manageRemove(long myRank)
{
	int row = 0;
	int col = 0;

	// First thread not done and add queue is not empty
	while ((done != 1) || !(isEmpty (&removeQue)))
	{
		// There is something in the remove queue
		if (!(isEmpty (&removeQue)))
		{
			// remove entry from queue
			pthread_mutex_lock(&removeMutex);
			dequeue (&removeQue, &row, &col);
			pthread_mutex_unlock(&removeMutex);

			// Add remove cell to new board
			if (curGrid == 0)
			{
				gridTwo[row][col] = 0;
			}
			else
			{
				gridOne[row][col] = 0;
			}
		}

		if (DEBUG3)
			printf  ("Thread %ld has gone insane\n", myRank);
	} // end of outer while loop
} // End of manageRemove

/*
	- Goes through each grid and frees their rows. Finish
	off by freeing the grids.
	@Arg:
	N/A
	@Return:
	N/A
*/
static void freeGrid()
{
	int i;

	for (i = 0; i < gridSize; i++)
	{
		free(gridOne[i]);
		free(gridTwo[i]);
	}

	free(gridOne);
	free (gridTwo);
} // End of freeGrid

/*
	- Frees all memory allocated in program.
	@Arg:
	N/A
	@Return:
	N/A
*/
void freeAll (pthread_t* thread_handles)
{
	free (thread_handles);
	pthread_mutex_destroy (&countMutex);
	pthread_mutex_destroy (&addMutex);
	pthread_mutex_destroy (&removeMutex);
	pthread_cond_destroy(&allToGet);
	freeGrid ();		// Frees the grids.
	destroyQue (&addQue);
	destroyQue (&removeQue);
} // End of freeAll
