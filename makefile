CFLAGS = -Wall -pedantic -std=c99 -g

Everything:	data task

task:	task.c taskFunc.o queue.o
		gcc $(CFLAGS) queue.o taskFunc.o task.c -o task -lpthread

taskFunc.o:	taskFunc.h taskFunc.c
	gcc $(CFLAGS) taskFunc.c -o taskFunc.o -c

queue.o:	queue.h queue.c
	gcc $(CFLAGS) queue.c -o queue.o -c

data:	data.c dataFunc.o
	gcc $(CFLAGS) dataFunc.o data.c -o data -lpthread

dataFunc.o:		dataFunc.h dataFunc.c
	gcc $(CFLAGS) dataFunc.c -o dataFunc.o -c

memory:
	valgrind --leak-check=yes --track-origins=yes ./data 4 10 100

memory2:
	valgrind --leak-check=yes --track-origins=yes ./task 6 2 -d

run:
	./data 4 10 100

run2:
	./data 2 100 5 -d

run3:
	./task 6 2

run4:
	./task 6 2 -d

test:
	./data 2 4 5 -d

test2:
	./task 10 10

test3:
	./task 10 10 -d

clean:
	rm -f *.o task data

save:
	tar czf CIS3090A1moranski.tar.gz task.c taskFunc.c taskFunc.h queue.c queue.h data.c dataFunc.c dataFunc.h makefile README.txt

delete:
	rm -f *.gz
