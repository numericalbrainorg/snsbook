all: run

CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

hh: hh.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

hh1: hh1.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

fr: fr.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

sfa: sfa.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

ia: ia.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

.c.o:
	$(CC) $(CFLAGS) -c $<

run: hh hh1 fr sfa ia
	./hh > hh.dat
	./hh1 > hh1.dat
	./fr > fr.dat
	./sfa > sfa.dat
	./ia > ia.dat

clean:
	rm -f hh hh1 fr sfa ia *.o *~

distclean: clean
	rm -f *.dat
