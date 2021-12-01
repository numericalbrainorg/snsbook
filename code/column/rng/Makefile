CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -DSFMT_MEXP=19937

all: main_rand main_sfmt

main_rand: main_rand.o
	$(CC) $(CFLAGS) -o $@ $^

main_rand.o: main_rand.c
	$(CC) $(CFLAGS) -c $<

main_sfmt: main_sfmt.o SFMT.o
	$(CC) $(CFLAGS) -o $@ $^

main_sfmt.o: main_sfmt.c $(SFMTDIR)/SFMT.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c $(SFMTDIR)/SFMT.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

clean:
	rm -f main_rand main_sfmt *.o

distclean: clean
