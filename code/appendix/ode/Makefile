all: run

CC = gcc
CFLAGS = -O3 -std=gnu11 -Wall

car: car.o
	$(CC) $(CFLAGS) -o $@ $^

car2: car2.o
	$(CC) $(CFLAGS) -o $@ $^

.c.o:
	$(CC) $(CFLAGS) -c $<

run: car car2
	./car > car.dat
	./car2 > car2.dat

clean:
	rm -f car car2 *.o

distclean: clean
	rm -f *.dat
