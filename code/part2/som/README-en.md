# 10. Self-organizing map (SOM)

## Files
- `som.c`: Code of SOM
- `mnist.c`: Functions to read/write MNIST data
- `mnist.h`: Header file for `mnista.c`
- `./mnist/`: Directory to store MNIST data
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English

## Description

So-called SOM is implemented by a spiking network model to explain Time-to-First-Spike (TFS).

Because MNIST data are not included, we must download them first from Yann LeCun's [MNIST page](http://yann.lecun.com/exdb/mnist/):
```
train-images-idx3-ubyte.gz:  training set images (9912422 bytes)
train-labels-idx1-ubyte.gz:  training set labels (28881 bytes)
t10k-images-idx3-ubyte.gz:   test set images (1648877 bytes)
t10k-labels-idx1-ubyte.gz:   test set labels (4542 bytes)
```
and put them under the directory `./mnist/`. Do not forget to gunzip those files.

## How to compile
Do `make`.

## How to run
When run `./som`, the number of simulation steps and the presented digit number at the step are shown. After 60,000 steps,
the calculation terminates. Reference vectors are visualized in `w.png`, which is identical to Figure 10.2.
