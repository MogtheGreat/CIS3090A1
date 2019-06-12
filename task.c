/* taskFunc.c

Uses task parallism to run the game of life using threads.

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/

#include "taskFunc.h"

int main (int argc, char * argv[])
{
	int i;
	int iterate;					// Number of iterations
	long thread;					// Number of threads
	int display;					// Booolean. Display grid or not.
	pthread_t* thread_handles;

	// Checks to make sure if all arguements in the command line is valid.
	if (!(checkArg(argc, argv)))
	{
		return EXIT_FAILURE;
	}

	// Tries to initialize the global variables and assign values to them
	if (!(initGlobal (argv)))
	{
		printf ("Critical Failure! Trouble initializing global variables!\n");
		return EXIT_FAILURE;
	}

	iterate = atoi(argv[2]);

	if (argc == 4)
		display = 1;
	else
		display = 0;

	thread_handles = malloc (sizeof(pthread_t) * 3);
	fillGrid ();			// Fills both boards (girds) with zeros
	createStart ();			// Creates a starting population on gird one

	// For display flag, displays the first board before the threads start on it
	if (display == 1)
	{
		curGrid = 1;
		displayGrid ();
		curGrid = 0;
	}

	for (i = 0; i < iterate; i++)
	{
		// Create all the threads and assign them various jobs
		for (thread = 0; thread < 3; thread++)
		{
			pthread_create (&thread_handles[thread], NULL, startTasks,(void*) thread);
		}

		// Collect all the threads
		for (thread = 0; thread < 3; thread++)
		{
			pthread_join (thread_handles[thread], NULL);
		}

		// Display the new board state
		if (display == 1)
			displayGrid ();

		// Shift focus to the next board.
		curGrid = (curGrid + 1) % 2;
	}

	freeAll (thread_handles);	// Frees all malloc'd memory

	return EXIT_SUCCESS;
}
