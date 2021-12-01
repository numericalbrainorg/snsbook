all: run

CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

lif: lif.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

lif2: lif2.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

network: network.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

network_delay: network_delay.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

lif_alt: lif_alt.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

lif_refr: lif_refr.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
.c.o:
	$(CC) $(CFLAGS) -c $<

run: lif lif2 network lif_alt lif_refr network_delay
	./lif  > lif.dat
	./lif2 > lif2.dat
	./network > network.dat
	./lif_alt  > lif_alt.dat
	./lif_refr  > lif_refr.dat
	./network > network_delay.dat

clean:
	rm -f lif lif2 network lif_alt lif_refr network_delay *.o *~

distclean: clean
	rm -f *.dat
