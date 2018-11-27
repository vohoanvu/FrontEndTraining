TARGET = project2
LIBS = -lm -pthread
CC = gcc
CFLAGS = -g -Wall

all: project2

iterator.o: iterator.c
	$(CC) $(CFLAGS) -c $< -o $@

bqueue.o: bqueue.c
	$(CC) $(CFLAGS) -c $< -o $@

tsiterator.o: tsiterator.c
	$(CC) $(CFLAGS) -c $< -o $@

tsbqueue.o: tsbqueue.c
	$(CC) $(CFLAGS) -c $< -o $@

main.o: main.c
	$(CC) $(CFLAGS) -c $< -o $@


project2: iterator.o bqueue.o tsiterator.o tsbqueue.o main.o
	$(CC) -Wall $(LIBS) $^ -o $@


clean:
		rm -f *.o
		rm -f project2
