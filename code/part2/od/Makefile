CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -DSFMT_MEXP=19937
MISC_OBJS = SFMT.o

all: run

od: od.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

od.o: od.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

run: od
	./od

clean:
	rm -f od *.o

distclean: clean
	rm -f *.dat
