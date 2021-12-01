CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

all: main

main: main.o skel.o
	$(CC) $(CFLAGS) -o $@ $^

main.o skel.o: skel.h

.c.o: 
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f main *.o
