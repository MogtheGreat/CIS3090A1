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

int gridSize;
int** gridOne;
int** gridTwo;
int curGrid;
int countThr;
int done;
pthread_mutex_t countMutex;
pthread_mutex_t addMutex;
pthread_mutex_t removeMutex;
pthread_cond_t allToGet;
cellStatus addQue;
cellStatus removeQue;

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
