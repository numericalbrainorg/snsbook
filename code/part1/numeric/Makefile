CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

all: run

exp: exp.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

run: exp
	./exp

clean:
	rm -f exp *.o

distclean: clean
	rm -f *.dat
