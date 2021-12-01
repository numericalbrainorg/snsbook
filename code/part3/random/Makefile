CC = gcc
MPICC = mpicc
NVCC = nvcc
CFLAGS = -O3 -std=gnu11 -Wall
OMPFLAGS = -fopenmp
NVCCFLAGS = -O3
SFMTDIR = ../misc/SFMT-src-1.5.1
SFMTFLAGS = -I$(SFMTDIR) -D SFMT_MEXP=19937
MISC_OBJS = SFMT.o timer.o

all: random random_ell random_omp random_mpi random_mpi_ws random_hyb random_gpu random_gpu_reduction

random: random.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_ell: random_ell.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_omp: random_omp.o $(MISC_OBJS)
	$(CC) $(CFLAGS) $(OMPFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_mpi: random_mpi.o $(MISC_OBJS)
	$(MPICC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_mpi_ws: random_mpi_ws.o $(MISC_OBJS)
	$(MPICC) $(CFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_hyb: random_hyb.o $(MISC_OBJS)
	$(MPICC) $(CFLAGS) $(OMPFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_gpu: random_gpu.o $(MISC_OBJS)
	$(NVCC) $(NVCCFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random_gpu_reduction: random_gpu_reduction.o $(MISC_OBJS)
	$(NVCC) $(NVCCFLAGS) $(SFMTFLAGS) -o $@ $^ -lm

random.o: random.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

random_ell.o: random_ell.c
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

random_omp.o: random_omp.c
	$(CC) $(CFLAGS) $(OMPFLAGS) $(SFMTFLAGS) -c $<

random_mpi.o: random_mpi.c
	$(MPICC) $(CFLAGS) $(SFMTFLAGS) -c $<

random_mpi_ws.o: random_mpi_ws.c
	$(MPICC) $(CFLAGS) $(SFMTFLAGS) -c $<

random_hyb.o: random_hyb.c
	$(MPICC) $(CFLAGS) $(OMPFLAGS) $(SFMTFLAGS) -c $<

random_gpu.o: random_gpu.cu
	$(NVCC) $(NVCCFLAGS) $(SFMTFLAGS) -c $<

random_gpu_reduction.o: random_gpu_reduction.cu
	$(NVCC) $(NVCCFLAGS) $(SFMTFLAGS) -c $<

SFMT.o: $(SFMTDIR)/SFMT.c $(SFMTDIR)/SFMT.h
	$(CC) $(CFLAGS) $(SFMTFLAGS) -c $<

timer.o: ../misc/timer.c
	$(CC) $(CFLAGS) -c $<

clean:
	rm -f random random_ell random_omp random_mpi random_mpi_ws random_hyb random_gpu random_gpu_reduction *.o

distclean: clean
	rm -f *.dat
