`-*- coding: utf-8 -*-`

# A.1 Numerical simulation for high-school students

## Files

- `car.c`: Simulation of constant velocity motion (List A.1)
- `car2.c`: Simulation of constant acceleration motion (List A.2)
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## How to compile
Just follow the textbook:
```
node00:~/snsbook/code/part1/appendix/ode$ gcc -O3 -std=gnu11 -Wall -o car car.c
node00:~/snsbook/code/part1/appendix/ode$ gcc -O3 -std=gnu11 -Wall -o car2 car2.c
```

Or, doing `make` compiles and runs all:
```
node00:~/snsbook/code/part1/appendix/ode$ make
gcc -O3 -std=gnu11 -Wall -c car.c
gcc -O3 -std=gnu11 -Wall -o car car.o
gcc -O3 -std=gnu11 -Wall -c car2.c
gcc -O3 -std=gnu11 -Wall -o car2 car2.o
./car > car.dat
./car2 > car2.dat
node00:~/snsbook/code/part1/appendix/ode$
```
Results are stored in `car.dat` and `car2.dat`, respectively.

## How to run
Follow the textbook and do `car` and `car2`.
