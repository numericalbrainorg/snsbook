# 5. Simulation of ocular dominance map formation in the primary visual cortex

## Files
- `od.c`: Model of ocular dominance map formation
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
Follow Section 5.3.3 of the textbook and do `make`. When doing `make`, both compiling and running the code
are carried out. To only compile, do as follows:
```
node00:~/snsbook/code/part2/od$ make od
```

## How to run
Follow the textbook, and plot `before.dat` and `after.dat`. Figure 5.6 will be obtained.

Simulation of monocular deprivation is carried out by setting the values of array `activity` as:
```
//float activity [ N_PRE ] = { 1.0, 1.0 };
float activity [ N_PRE ] = { 1.0, 0.5 };
```
When setting at 0.5 and 0.1, Figures 5.7a and 5.7b will be obtained, respectively.
