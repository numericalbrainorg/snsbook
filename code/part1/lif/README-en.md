# 3.2 Simulation of leaky integrate-and-fire model

## Files
- `lif.c`: Leaky integrate-and-fire model (List 3.2)
- `lif_alt.c`: Another implementation of LIF model (List 3.3)
- `lif_refr.c`: LIF model with refractory period (List 3.4)
- `lif2.c`: Two LIF models (List 3.5)
- `network.c`: Network of two LIF models (List 3.6)
- `network_delay.c`: Network of two LIF models with refractory period and synaptic delay (List 3.7)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
Follow the textbook, or just type `make` to compile and run all simulations.

## How to run
Follow the textbook. Figures 3.7, 3.11, 3.12 will be obtained from the  results of `lif`, `lif2`,
and `network`, respectively. To reproduce Figures 3.12a and 3.12b, the synaptic weight defined as:
```
#define W       (    2.0 ) // connection weight
```
should be changed from 2.0 to -2.0, and also the initial membrane potentials:
```
float v [ N ] = { V_REST, V_REST - 15. };
```
from `V_REST`-15 to `V_REST`-1.

When `make`, the results will be put in `lif.dat`, `lif_alt.dat`, `lif_refr.dat`, `lif2.dat`, `network.dat`, and `network_delay.dat`.
