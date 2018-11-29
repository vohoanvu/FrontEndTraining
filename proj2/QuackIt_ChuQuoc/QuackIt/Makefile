TARGET = startQuacker 
LIBS = -lm 
CC = gcc
CFLAGS = -g -Wall
LDFLAGS= -pthread

all: startQuacker 

circular_buffer.o: circular_buffer.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

startQuacker.o: startQuacker.c
	$(CC)  $(CFLAGS) $(LDFLAGS) -c $< -o $@

startQuacker: startQuacker.o 
	$(CC) -Wall $(LIBS) $^ -o $@ -lpthread


clean:
		rm -f *.o
