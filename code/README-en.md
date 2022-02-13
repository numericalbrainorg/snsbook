`-*- coding: utf-8 -*-`

# README-en.md

This folder contains the source codes of spiking network simulation used in the following textbook:

> Tadashi Yamazaki, Jun Igarashi. Introduction to Spiking Network SImulation (in Japanese). Morikita Publishing. 2021
[(Link to the publisher's site)](https://www.morikita.co.jp/books/mid/085631)

## Folder structure

Appendix A.3 of the textbook provides the list of subfolders. The list is also attached at the tail of this text.
Each subfolder contains `README` and `Makefile`.

For pseudo random number generator, [SIMD-oriented Fast Mersenne Twister (SFMT)](http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/index-jp.html) of Version 1.5.1 is used throughout the text, which are stored at `./{column, part1, part2, part3}/misc/SFMT-1.5.1`.

## How to use

Read the instructions in the textbook, and also refer to `README-en.md` in each subfolder.

## License

- SFMT is licensed under the modified BSD license. The details are found in `LICENSE.txt` under `SFMT-1.5.1`. Or, confirm [`LICENSE.txt`](http://www.math.sci.hiroshima-u.ac.jp/m-mat/MT/SFMT/LICENSE.txt) on the SFMT's website.
- `./{column, part1, part2, part3}/misc/timer.c` are public domain.
- The rest are licensed under the modified BSD license. See `LICENSE.txt`.

## Files

```
part1/ : Source codes for Part I
part1/hh/ : Hodgkin-Huxley-type models
part1/hh/hh.c : The Hodgkin-Huxley model
part1/hh/sfa.c : Spike frequency adaptation model
part1/hh/ia.c : Type I neuron model
part1/lif/ : Leaky integrate-and-fire model
part1/lif/lif.c : 1 LIF model
part1/lif/lif_alt.c : Another implementation of 1 LIF model
part1/lif/lif_refr.c : LIF model with refractory period
part1/lif/lif2.c : 2 LIF models
part1/lif/network.c : Network of 2 LIF models
part1/lif/network_delay.c : Network of 2 LIF models with refractory period and synaptic delay
part1/random/ : Random networks
part1/random/random.c : Random network model
part1/multi/ : Multicompartment models
part1/multi/traub.c : Hippocampal CA3 model
part1/numeric/ : Numerical methods to solve ordinary differential equations
part1/numeric/exp.c : Initial value problem to obtain the value of exp(1)
part2/ : Source codes for Part II
part2/od/ : Ocular dominance map formation
part2/ec/ : Delay eyeblink conditioning
part2/bg/ : GridWorld
part2/hopfield/ : Associative memory
part2/cpg/ : Central pattern generator (CPG)
part2/biped/ : Bipedal locomotion
part2/som/ : Self-organizing map (SOM)
part2/sudoku/ : Sudoku
part3/ : Source codes for Part III
part3/random/random.c : Original random network model
part3/random/random_ell.c : Modified ELL version
part3/random/random_omp.c : OpenMP version
part3/random/random_mpi.c : MPI version
part3/random/random_mpi_ws.c : Weak scaling
part3/random/random_hyb.c : Hybrid version
part3/random/random_gpu.cu : GPU version
part3/random/random_gpu_reduction.cu : GPU version with parallel reduction of synapses
part3/random/hostfile : Hostfile for the MPI version
column/ : Source codes for Columns
column/make/ : How to write Makefile
column/rng/ : Pseudo random number generator
column/soa/ : SoA vs AoS
appendix/ : Source codes for Appendices
appendix/ode/ : Numerical simulation for high-school students
appendix/ode/car.c : Simulation of constant velocity movement
appendix/ode/car2.c : Simulation of constant acceleration movement
```
