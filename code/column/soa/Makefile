CC = nvcc
CFLAGS = -O3 --generate-code arch=compute_70,code=sm_70

all: soa aos

soa: soa.o hh.o timer.o
	$(CC) $(CFLAGS) -o $@ $^ -lm
aos: aos.o hh.o timer.o
	$(CC) $(CFLAGS) -o $@ $^ -lm

soa.o: soa.cu hh.h
	$(CC) $(CFLAGS) -c $<
aos.o: aos.cu hh.h
	$(CC) $(CFLAGS) -c $<
hh.o: hh.cu hh.h
	$(CC) $(CFLAGS) -c $<
timer.o: ../misc/timer.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f soa aos *.o *~

distclean: clean
