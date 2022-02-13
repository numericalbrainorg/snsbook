# 11. Sudoku

## Files
- `sudoku.c`: Spiking network model to solve Sudoku
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## Description
Nothing particularly.

## How to compile
Just do `make`.

## How to run
Follow the instruction in the textbook. The result will be shown on the standard output.

To reproduce Figure 11.3, first enable the following line:
```
//if ( s [ i ] ) { printf ( "%d %d\n", nt, i ); } // print spikes for raster plots
```
Next, compile the code and run as:
```
node00:~/snsbook/code/part2/sudoku$ ./sudoku > sudoku.dat
```
Then, plot `sudoku.dat`.
