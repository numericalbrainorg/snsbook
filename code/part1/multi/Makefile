CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

all: run

traub: traub.o
	$(CC) $(CFLAGS) -o $@ $< -lm

.c.o:
	$(CC) $(CFLAGS) -c $^

run: traub
	./traub > traub.dat

clean:
	rm -f traub *.o *~

distclean: clean
	rm -f *.dat
