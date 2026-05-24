all: sequential_min_max parallel_min_max executor

executor: executor.c
	gcc executor.c -o executor

sequential_min_max: find_min_max.o sequential_min_max.o utils.o
	gcc find_min_max.o sequential_min_max.o utils.o -o sequential_min_max -lm

parallel_min_max: find_min_max.o parallel_min_max.o utils.o
	gcc find_min_max.o parallel_min_max.o utils.o -o parallel_min_max -lm -lpthread

find_min_max.o: find_min_max.c find_min_max.h
	gcc -c find_min_max.c -o find_min_max.o

sequential_min_max.o: sequential_min_max.c find_min_max.h utils.h
	gcc -c sequential_min_max.c -o sequential_min_max.o

parallel_min_max.o: parallel_min_max.c find_min_max.h utils.h
	gcc -c parallel_min_max.c -o parallel_min_max.o

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

clean:
	rm -f *.o sequential_min_max parallel_min_max executor

.PHONY: all clean
all: sequential_min_max parallel_min_max

sequential_min_max: find_min_max.o sequential_min_max.o utils.o
	gcc find_min_max.o sequential_min_max.o utils.o -o sequential_min_max -lm

parallel_min_max: find_min_max.o parallel_min_max.o utils.o
	gcc find_min_max.o parallel_min_max.o utils.o -o parallel_min_max -lm -lpthread

find_min_max.o: find_min_max.c find_min_max.h
	gcc -c find_min_max.c -o find_min_max.o

sequential_min_max.o: sequential_min_max.c find_min_max.h utils.h
	gcc -c sequential_min_max.c -o sequential_min_max.o

parallel_min_max.o: parallel_min_max.c find_min_max.h utils.h
	gcc -c parallel_min_max.c -o parallel_min_max.o

utils.o: utils.c utils.h
	gcc -c utils.c -o utils.o

clean:
	rm -f *.o sequential_min_max parallel_min_max

.PHONY: all clean
CC=gcc
CFLAGS=-I.

sequential_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o sequential_min_max find_min_max.o utils.o sequential_min_max.c $(CFLAGS)

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h
	$(CC) -o parallel_min_max utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)

find_min_max.o : utils.h find_min_max.h
	$(CC) -o find_min_max.o -c find_min_max.c $(CFLAGS)

clean :
	rm utils.o find_min_max.o sequential_min_max parallel_min_max
