CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -DSFMT_MEXP=19937
MISC_OBJS = SFMT.o mnist.o
GDFLAGS = -I/usr/local/include -L/usr/local/lib

all: som

som: som.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(SFMTFLAGS) $(GDFLAGS) -o $@ $^ -lm -lgd

som.o: som.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

mnist.o: mnist.c
	$(CC) $(CFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ -c $<

clean:
	rm -f som *.o

distclean: clean
	rm -f *.dat *.png
