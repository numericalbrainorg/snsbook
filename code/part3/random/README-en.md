# Part III Using supercomputers efficiently

## Files
- `random.c`: Random network model (identical to `code/part1/random/random.c`)
- `random_ell.c`: ELL version of `random.c` (Lists 12.1, 12.2)
- `random_omp.c`: OpenMP version (Lists 13.2, 13.3)
- `random_mpi.c`: MPI version (Lists 14.1--17)
- `random_mpi_ws.c`: Code for checking weak scaling (Figure 14.4)
- `random_hyb.c`: Hybrid parallel version with OpenMP and MPI
- `random_gpu.cu`: GPU version (Lists 15.1--13)
- `random_gpu_reduction.cu`: GPU version with parallel reduction of synapses (Lists 15.14--21)
- `hostfile`: Hostfile used for the MPI version (List 14.1)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-ja.md`: Description in English (this file)

## Description
Comparing computational time by parallelizing `random.c` in various ways.
- `random_omp.c` requires a C compiler that supports OpenMP (for gcc, version 4.9 or later).
- `random_mpi.c` requires MPI. [mvapich2](https://mvapich.cse.ohio-state.edu/) is used in the textbook.
- `random_hyb.c` requires both OpenMP and MPI.
- `random_gpu.cu` and `random_gpu_reduction.cu` require NVIDIA GPU and CUDA with Compute Capability 6 or above.

## How to compile
Do `make xxxxx` to compile `xxxxx.c`. For exmample, `make random_ell` compiles `random_ell.c`.

## How to run

For `random_ell` and `random_omp`, just do so. For `random_omp`, the number of threads can be specified by
```
node01:~/snsbook/code/part3/random$ OMP_NUM_THREADS=24 ./random_omp
# of omp threads = 24
Elapsed time = 0.445473 sec.
```

For `random_mpi`, follow the textbook and do as follows:
```
node00:~/snsbook/code/part3/random$ mpirun -hostfile hostfile -np 192 ./random_mpi
```
`-np` specifies the number of MPI processes. So, by changing the number 1, 2, 4, 8, ..., Figure 14.3 will be obtained.

Running `random_mpi_ws` might be a bit complicated. The number of neurons is specified at the head of the code:
```
//#define N       ( 4000 )            // # of neurons
//#define N_E     ( 3200 )            // # of excitatory neurons
#define N       ( 8 * 4000 )            // # of neurons
#define N_E     ( 8 * 3200 )            // # of excitatory neurons
```
To obtain Figure 14.4, we must change the numbers of neurons and processes simultaneously as
4000 neurons and 8 processes, 8000 neurons and 16 process, and so on.

For `random_hyb`, follow the textbook as:
```
node00:~/snsbook/code/part3/random$ mpirun -hostfile hostfile -np 16 ./random_hyb
```

For `random_gpu`, just type `random_gpu`.

On `random_gpu_reduction`, `calculateSynapticInputs_a` is used as default:
```
calculateSynapticInputs_a <<< GRID_SIZE, BLOCK_SIZE >>> ( n );
//calculateSynapticInputs_b <<< N, NTHSYN >>> ( n );
```
To enable the parallel reduction of synapses, use `calculateSynapticInputs_b` while commenting out
`calculateSynapticInputs_a`.
