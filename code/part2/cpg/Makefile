lCC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

all: run

cpg: cpg.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

run: cpg
	./cpg > cpg.dat

clean:
	rm -f cpg *.o

distclean: clean
	rm -f *.dat
