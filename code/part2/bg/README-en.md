# 7. Simulation of GridWorld

## Files
- `gw.c`: GridWorld
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## Description
This code reimplements Fr\'{e}maux et al. (2013). The original paper uses SRM0 as a neuron model (because the paper
is from Gerstner lab.), the present code uses a LIF model. The original paper is published as an open access license,
so please check the details.

[Fr\'{e}maux et al. (2013) Reinforcement Learning Using a Continuous Time Actor-Critic Framework with Spiking Neurons. PLoS Comput Biol 9(4): e1003024.](https://journals.plos.org/ploscompbiol/article?id=10.1371/journal.pcbi.1003024)

## How to compile
Follow section 7.3 and `make`, or to only compile do
```
node00:~/snsbook/code/part2/bg$ make gw
```

## How to run
Follow the textbook. Figures 7.4a, 7.4b, 7.4c and d will be obtained by plotting
`pos.dat`, `td.dat`, and `raster.dat`, respectively.

## Acknowledgement
This code is contributed by Mr. Hideyuki Yoshimura.
