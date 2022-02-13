# 6. Simulation of delay eyeblink conditioning

## Files
- `main.c`: main function
- `gr.c`: Granule cell model
- `gr.h`: Granule cell model (parameters)
- `go.c`: Golgi cell model
- `go.h`: Golgi cell model (parameters)
- `pc.c`: Purkinje cell model
- `pc.h`: Purkinje cell model (parameters)
- `conn.c`: Connections between granule and Golgi cells
- `conn.h`: Connections between granule and Golgi cells (parameters)
- `param.h`: Various global parameters
- `psth.rb`: Ruby script to calculate PSTH (Figure 6.5)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-ja.md`: Description in English (this file)

## Description
This code is a simplified and refactored version of Yamazaki & Nagao (2012) and Yamazaki & Tanaka (2007), which are
open access articles so that you can download and see the details. The former carries out simulation eyeblink conditioning,
whereas the latter gain adaptation of optokinetic response eye movements.
[Yamazaki & Nagao (2012) A Computational Mechanism for Unified Gain and Timing Control in the Cerebellum. PLoS ONE 7(3): e33319.](https://journals.plos.org/plosone/article?id=10.1371/journal.pone.0033319)
[Yamazaki & Tanaka (2007) A spiking network model for passage-of-time representation in the cerebellum. European Journal of Neuroscience 26(8): 2279--2292.](https://onlinelibrary.wiley.com/doi/10.1111/j.1460-9568.2007.05837.x)

The basic dynamics of the network model is as follows: first, the granular layer network composed of granule cells and Golgi cells as a recurrent inhibitory network generates complex spatiotemporal activity patterns of granule cells, which can be used as overcomplete bases. By combining them with LTD/LTP
at parallel fiber-Purkinje cell synapses, only synaptic weights from granule cells that act at the onset of airpuff are weaken, and thus stopping excitatory inputs to Purkinje cells at the timing. There are 102,400 granule cells, 1,024 Golgi cells, and 16 Purkinje cells. Granule cells constitute clusters so that
long passage of time is represented stably across trials.

## How to compile
Follow Section 6.3.1 of the textbook and do `make` to compile and run. To only compile, do as:
```
node00:~/snsbook/code/part2/ec$ make main
```

## How to run
By running the code, various files are generated under the names of `gr.spk.n`, `go.spk.n`, `pc.mbp.n`, `pc.spk.n`, where
`n` is the trial number.

Raster plot of granule cells is obtained by plotting `gr.spk.n`. Figure 6.4 will be obtained. In the simulation,
tone stimulus is fed after 250 ms from the start of the trial, but in Figure 6.4, the onset of the tone is shifted to 0 ms.
Raster plots of Golgi cells and Purkinje cells are also obtained similarly by plotting `go.spk.n` and `pc.spk.n`.

PSTH of granule cells will be obtained by running `psth.rb`. The scrips generates two files: `gr.trial.n`
for spike patterns of granule cells 1 to 8 (the number is 0 to 7) across trials (`n` is the neuron number),
and `gr.psth.n` for the PSTH. By plotting `gr.trial.n`, Top panels of Figure 6.5 will be obtained, whereas
by plotting `gr.psth.n`, the bottom panels will be obtained. The bin size of the PSTH is defined at the
head of the script as:
```
B = 10 # Bin size of the PSTH (ms)
```
To plot the PSTH by gnuplot, use `with impulses` (or `w i` for short) and plot the data by bar graphs.

Spike patterns of Purkinje cells are shown by plotting `pc.mbp.n`. Figure 6.7 will be reproduced except
the vertical bars representing spikes. To reproduce the spikes as well, modify the code to put the vertical
bars simultaneously, or write a script that generates appropriate data from `pc.mbp.n` and `pc.spk.n`
(Figure 6.7 was obtained by the latter way).
