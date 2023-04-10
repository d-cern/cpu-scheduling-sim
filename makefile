CC = gcc
CFLAGS = -Wall -g

all: prog

clean:
	rm -f *.o prog

linkedlist.o: linkedlist.c linkedlist.h
	$(CC) $(CFLAGS) -c linkedlist.c

prog.o: prog.c prog.h
	$(CC) $(CFLAGS) -c prog.c

prog: prog.o linkedlist.o
	$(CC) $(CFLAGS) prog.o linkedlist.o -o prog


