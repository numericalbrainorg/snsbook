# 3.5.4. Simulation of a random network

## Files
- `random.c`: Random network model (List 3.8)
- `popavg.rb`: Ruby script to calculate the population average (Figure 3.16)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
Follow the textbook, or just type `make` to compile and execute the code. If you want to
just compile: do
```
node00:~/snsbook/code/part1/random$ make random
```

## How to run
Follow the textbook and plot `spike.dat`. Figure 3.13 will be obtained.

To calculate the population average, use `popavg.rb`. Define the bin size at the head of
the script appropriately as
```
B = 10 # Bin size (ms)
```
and run. Then, `popavg.dat` will be obtained after execution. By plotting this file, Figure 3.16 will
be obtained. Here, the data should be plotted as a bar graph by `with implulses` (or `w i` for short)
when using gnuplot.
