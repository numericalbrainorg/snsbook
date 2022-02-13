# 8. Simulation of (auto-) associative memory

## Files
- `hopfield.c`: Code of the Hopfield model
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## Description
The so-called Hopfield model is implemented by a spiking network model to explain crosstalk and cell assembly.

## How to compile
Follow Section 8.2.1 of the textbook and do `make` to compile and run. To only compile, do as:
```
node00:~/snsbook/code/part2/hopfield$ make hopfield
```

## How to run
Follow the textbook. Activity of the network that embeds patterns shown in Figure 8.2b will be calcualted in response to the pattern shown in Figure 8.2c.
The values of the firing rate will be put on the standard output in the following format:
```
row-number column-number firing-rate-of-the-neuron-at-that-position (spikes/s)
```
From the output, Figures 8.3a, b will be produced. On the other hand, the membrane potential of neuron `n` will be
put in `n.dat` (n is a number). Plotting the file will result in Figure 8.3c. Specifically,
```
plot [800:1000][-70:10] '2.dat' t '' w l, '7.dat' t '' w l, '12.dat' t '' w l, '13.dat' t '' w l, '14.dat' t '' w l, '17.dat' t '' w l, '22.dat' t '' w l
```

Patterns to be fed are set by function `setInput`. In the present code, the function is as follows. The left half of pattern 1 (`p1`, + symbol) is fed.
By rewriting the function, other patterns can be fed to the network.
```
void setInput ( float * i_ext )
{
  for ( int32_t i = 0; i < NX; i++ ) {
    for ( int32_t j = 0; j < NY; j++ ) {
      i_ext [ j + NY * i ] = ( j < NX / 2 ) ? I_EXT * p1 [ i ] [ j ] : 0;
    }
  }
}
```
