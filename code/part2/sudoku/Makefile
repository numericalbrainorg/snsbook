CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall 
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -DSFMT_MEXP=19937

all: sudoku

sudoku: sudoku.o SFMT.o
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

sudoku.o: sudoku.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

clean:
	rm -f sudoku *.o

distclean: clean
	rm -f *.dat
