

TARGET = gambler bulk_scheduler
LIBS = -lm
CC = gcc
CFLAGS = -g -Wall


all: gambler bulk_scheduler

gambler.o: gambler.c
	$(CC) $(CFLAGS) -c $< -o $@

bulk_scheduler.o: bulk_scheduler.c
	$(CC) $(CFLAGS) -c $< -o $@

gambler: gambler.o 
	$(CC) -Wall $(LIBS) $^ -o $@

bulk_scheduler: bulk_scheduler.o
	$(CC) -Wall $(LIBS) $^ -o $@

clean:
	rm -f *.o
	rm -f gambler bulk_scheduler
