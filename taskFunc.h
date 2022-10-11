/* taskFunc.c

Contains set of functions to assign roles to threads and control them.

Created: 1 Oct. 2017
Author: Michael G. Oranski ID: 0557701
Contact: moranski@mail.uoguelph.ca
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "queue.h"
#define DEBUG 0
#define DEBUG2 0
#define DEBUG3 0

#ifndef _TASKFUNC_H_
#define _TASKFUNC_H_

extern int gridSize;
extern int** gridOne;
extern int** gridTwo;
extern int curGrid;
extern int countThr;
extern int done;
extern pthread_mutex_t countMutex;
extern pthread_mutex_t addMutex;
extern pthread_mutex_t removeMutex;
extern pthread_cond_t allToGet;
extern cellStatus addQue;
extern cellStatus removeQue;

int checkArg (int argc, char * argv[]);

int initGlobal (char *argv[]);

void fillGrid ();

void createStart ();

void* startTasks (void* rank);

void checkGrid (long rank);

void manageAdd (long rank);

void manageRemove(long rank);

void displayGrid ();

void freeAll (pthread_t* thread_handles);

#endif
