# Column: AoS vs. SoA

## Files
- `aos.cu`: AoS version
- `soa.cu`: SoA version
- `hh.cu`: Functions for gate variables
- `hh.h`: Header file for the functions
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
Do `make` on a GPU machine with CUDA installed, and executables `aos` and `soa` will be built.
Compute Capability is specified at the head of `Makefile`. Current definition is for V100 as:
```
--generate-code arch=compute_70,code=sm_70
```
Set this appropriately for your GPUs. Meanwhile, Compute Capability 6 or later is mandatory,
because the code uses Unified Memory.

## How to run
Running `aos` and `soa` will perform simulation and terminates by showing the compute time.
Meanwhile, the loop function looks like:
```
kernel_gpu <<< gs, BLOCK_SIZE >>> ( neuron );
//kernel_cpu ( neuron );
```
The code will work as a CPU version when `kernel_cpu` is enabled instead of `kernel_gpu`.
