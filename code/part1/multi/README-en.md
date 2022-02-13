# 3.7 Simulation of a biophysical neuron model with spatial structure

## Files
- `traub.c`: Hippocampal CA3 model (List 3.8)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
When `make`, the code is compiled and executed. If you want to just compile, please try:
```
node00:~/snsbook/code/part1/multi$ make multi
```

## How to run
Follow the textbook and plot `traub.dat`. Figure 3.20a will be obtained by
```
gnuplot> plot 'traub.dat' using 1:10 with lines
```

To obtain Figure 3.20b, it is necessary to change the code slightly. First, extend the simulation time from 200 ms to 2000 ms as:
```
//#define T  ( 200   ) // ms
//#define NT ( 20000 ) // T / DT
#define T  ( 2000   ) // ms
#define NT ( 200000 ) // T / DT
```
and also inject current just 1 ms at 1000 ms from the start. as:
```
//i_inj [ 8 ] = 1.e-4 / area [ 8 ];
i_inj [ 8 ] = (  1000 <= t && t < 1001 ) ? 1.e-2 / area [ 8 ] : 0.;
```
Then, execute the code, and plot the membrane potentials of compartments for 50 ms from 1000 ms as:
```
gnuplot> plot [1000:1050] 'traub.dat' using 1:9 w l, 'traub.dat' using 1:12 w l, 'traub.dat' using 1:15 w l, 'traub.dat' using 1:18 w l
```
