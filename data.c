/* taskFunc.c

Use data parallelism to split the work of running game of life

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/

#include "dataFunc.h"

int main (int argc, char * argv[])
{
	long thread;
	int i;
	int check = 0;
	int iterate = 0;
	int display = 0;		// Boolean value. 1 (true) displays the current grid status
	pthread_t* thread_handles;

	// Not counting the executable as an arguement
	// Less then 3 arguements, the bare min
	if (argc < 4)
	{
		printf ("There must be at least 3 arguments in the command line!\n");
		printf ("The required arguments are: number of threads, grid-size, and the number of iterations.\n");
		return EXIT_FAILURE;
	}

	// More then 4 arguements, the most it should accept
	if (argc > 5)
	{
		printf ("There is too many arguments in the command line!\n");
		printf ("Please remove %d arguements.\n", (argc - 5));
		return EXIT_FAILURE;
	}

	// Checks to see if the arguements are the right type
	if (!(validateArg(argc, argv)))
	{
		printf ("Incorrect command line arguements!\n");
		printf ("Acceptable command line arguements: ./data # # # or ./data # # # -d\n");
		return EXIT_FAILURE;
	}

	// Checks to see if the value of the first 2 arguements are the right value
	check = atoi(argv[1]);

	if (check <= 0)
	{
		printf ("Value for thread number must be greater than 0.");
		return EXIT_FAILURE;;
	}

	check = atoi(argv[2]);

	if (check <= 0)
	{
		printf ("Value for grid size must be greater than 0.");
		return EXIT_FAILURE;;
	}

	check = atoi(argv[3]);

	if (check <= 0)
	{
		printf ("Value for number of iterations must be greater than 0.");
		return EXIT_FAILURE;;
	}

	//Initialize all global variables within the program
	threadNum = atoi(argv[1]);
	gridSize = atoi(argv[2]);
	iterate = atoi(argv[3]);
	curGrid = 0;

	if (DEBUG)
	{
		printf ("Grid Size is: %d\n", gridSize);
	}

	if (argc == 5)
		display = 1;

	thread_handles = malloc (sizeof(pthread_t) * threadNum);
	createGrid();		// Create the grids for the game
	fillGrid ();		// Fills both boards (girds) with zeros

	// Checks to see if testing grid
	if (TEST > 0)
	{
		createTest ();
	}
	else
	{
		createStart ();
	}

	// For display flag, displays the first board before the threads start on it
	if (display == 1)
	{
		curGrid = 1;
		displayGrid ();
		curGrid = 0;
	}

	for (i = 0; i < iterate; i++)
	{
		// Create all the threads and assign them same job of sorting through the data
		for (thread = 0; thread < threadNum; thread++)
		{
			pthread_create (&thread_handles[thread], NULL, checkCellLife,(void*) thread);
		}

		// Collect all the threads
		for (thread = 0; thread < threadNum; thread++)
		{
			pthread_join (thread_handles[thread], NULL);
		}

		// Display the new board state
		if (display == 1)
			displayGrid ();

		// Shift focus to the next board.
		curGrid = (curGrid + 1) % 2;
	}

	freeGrid();					// Frees the grids.
	free (thread_handles);

	return EXIT_SUCCESS;
}
