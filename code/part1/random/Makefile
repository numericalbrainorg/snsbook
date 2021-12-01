CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -DSFMT_MEXP=19937
MISC_OBJS = SFMT.o timer.o

all: run

random: random.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random.o: random.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c $(SFMTDIR)/SFMT.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

timer.o: ../misc/timer.c
	$(CC) $(CFLAGS) -c $<

run: random
	./random

clean:
	rm -f random *.o

distclean: clean
	rm -f *.dat
