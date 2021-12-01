CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

all: run

gw: gw.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

run: gw
	./gw

clean:
	rm -f gw *.o *~

distclean: clean
	rm -f *.dat
