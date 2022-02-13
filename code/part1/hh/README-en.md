# 3.1 Simulation of the Hodgkin-Huxley model

## Files
- `hh.c`: The Hodgkin-Huxley model (List 3.1)
- `hh1.c`: Modified List 3.1 code for Figure 3.2
- `fr.c`: Modified List 3.1 code for Figure 3.3
- `sfa.c`: Model of spike frequency adaptation
- `ia.c`: Type I neuron model (Conner-Stevens model)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
Follow the textbook, or just type `make` to compile everything automatically. When `make`,
all programs are executed after successful compilation. Execution of `fr` will take a time.

## How to run
On `hh`, just follow the text and put the result in a file. Figures 3.1a and 3.1b will be
obtained, when the result is plotted during [0:1000] ms and [900:1000] ms, respectively.n

On `hh1`, first try
```
node00:~/snsbook/code/part1/hh$ ./hh1 > hh1.dat
```
to put the result in a file, and then plot the result during [-5:25] ms. The format of the
file is as follows:
```
t V m h n
```
Using gnuplot, the membrane potential is plotted by
```
gnuplot> plot [-5:25] 'hh1.dat' using 1:2 title 'V' with lines
```
and m, h, and n are plotted by
```
gnuplot> plot [-5:25] 'hh1.dat' using 1:3 title 'm' with lines, 'hh1.dat' using 1:4 title 'h' with lines, 'hh1.dat' using 1:5 title 'n' with lines
```
When these plots are compiled appropriately, Figure 3.2 will be obtained.

On `fr`, there is no descriptions in the text, but it will calculate the firing rate while varying the amplitude of the external current.
The output format is as follows:
```
Current amplitude (\mu A / cm^2) Firing rate (spikes/s)
```
When plotted, Figure 3.3 will be obtained.

`sfa` can be executed similarly with `hh`. Figures 3.4a and 3.4b will be obtained by plotting membrane potentials
during [0:100] ms and [900:1000] ms, respectively.

`ia` is the same. The amplitude of the external current is set in a code as:
```
double I_ext = 8.60941453; // micro A / cm^2
```
So, Figures 3.5a and 3.6b will be obtained while changing the number. Furthermore, Figure 3.6 will be obtained
by calculating the firing rate while changing the amplitude as does `fr.c`.

When just typing `make`, results will be put in `hh.dat`, `hh1.dat`, `fr.dat`, `sfa.dat`, `ia.dat`, respectively.

After trying everything and being happy and contented, you can do
```
make distclean
```
to remove all generated files and back to the initial state (this behavior is common in all simulations).
