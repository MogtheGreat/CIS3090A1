/* taskFunc.c

Contains set of functions use threads to split up the work

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/
#include "dataFunc.h"

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
int validateArg (int argc, char * argv[])
{
	int i;
	int temp = -1;

	// Checks the first 3 arguments to see if they are ints
	for (i = 1; i < 4; i++)
	{
		temp = atoi(argv[1]);
		if (temp <= -1)
		{
			return 0;
		}
		temp = -1;
	}

	// Checks the last argument to see if it is -d
	if ((argc == 5) && (strcmp(argv[4], "-d") != 0))
	{
		return 0;
	}

	return 1;
} // End of validateArg

/*
	- Creates a board to the size indicated in the command line
	@Arg: N/A
	@Return:
	N/A
*/
void createGrid ()
{
	int i;

	gridOne = malloc (sizeof(int*) * gridSize);
	gridTwo = malloc (sizeof(int*) * gridSize);

	for (i = 0; i < gridSize; i++)
	{
		gridOne[i] = malloc (sizeof(int) * gridSize);
		gridTwo[i] = malloc (sizeof(int) * gridSize);
	}
} // End of createGrid ()

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

	srand((unsigned) time(&t)); // Initialize the random generator using time

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
} // End of createStarts

/*
	- Used for debugging purposes. By assigning TEST values,
	use static placement of live cells.
	@Arg:
	N/A
	@Return:
	N/A
*/
void createTest ()
{
	int end = gridSize - 1;

	// Only cornor cells alive
	if (TEST == 1)
	{
		gridOne[0][0] = 1;
		gridOne[end][end] = 1;
		gridOne[end][0] = 1;
		gridOne[0][end] = 1;
	}
	// Cells next to cornor alive
	else if (TEST == 2)
	{
		gridOne[0][1] = 1;
		gridOne[end][end-1] = 1;
		gridOne[1][end] = 1;
		gridOne[1][0] = 1;
	}
	// Only two cells alive, side by side
	else if (TEST == 3)
	{
		gridOne[1][0] = 1;
		gridOne[0][1] = 1;
	}
	// Only three cells alive, side by side
	else if (TEST == 4)
	{
		gridOne[1][0] = 1;
		gridOne[1][1] = 1;
		gridOne[0][1] = 1;
	}
	// Only three cells alive, side by sie, vertical shape
	else if (TEST == 5)
	{
		gridOne[2][1] = 1;
		gridOne[1][1] = 1;
		gridOne[0][1] = 1;
	}
} // End of createTest

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

	check = nearBoundry (row, col); // Checks to see if cell near the border of the grid
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
		// Cecks the neighbours around the right side (top to left to bottom)
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
	}

	if (DEBUG3)
	{
		printf  ("%d ", total);
	}

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
}

/*
	- Calculates which row to work on. Checks the cells 'alive' status on
	rows assigned to it.
*/
void* checkCellLife (void* rank)
{
	int i;
	int j;
	int end;
	long my_rank = (long) rank;
	int localHeight = gridSize / threadNum;	// Calculates the thread's local row range (height)
	int start = my_rank * localHeight;		// Calculate thread's starting point
	
	// If odd number of rows, assign reminder to last row
	if (my_rank == (threadNum -1))
	{
		end = ((my_rank + 1) * localHeight -1) + (gridSize % threadNum);
	}
	else
	{
		end = (my_rank + 1) * localHeight -1;
	}
	
	if (DEBUG)
	{
		printf ("Rank is: %ld\n", my_rank);
		printf ("my local height is: %d\n", localHeight);
		printf ("Start and end is: %d - %d\n", start, end);
	}

	for (i = start; i <= end; i++)
	{
		for (j = 0; j < gridSize; j++)
		{
			if (DEBUG2)
			{
				printf ("%d ", checkNeighbours (i, j, 0));
			}
			
			// Checks to see if cell is alive base on it's neighbours. Assign corresponding cell status to next grid
			if (curGrid == 0)
			{
				gridTwo[i][j] = checkNeighbours (i, j, gridOne[i][j]);
			}
			else
			{
				gridOne[i][j] = checkNeighbours (i, j, gridTwo[i][j]);
			}
		}
	} // end of outer for loop

	return NULL;
} // end of checkCellLife

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

	if (DEBUG4)
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
	- Goes through each grid and frees their rows. Finish
	off by freeing the grids.
	@Arg:
	N/A
	@Return:
	N/A
*/
void freeGrid()
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
