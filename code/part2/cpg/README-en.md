# 9.1. Simulation of central pattern generator (CPG)

## Files
- `cpg.c`: Code of CPG
- `Makefile`: Makefile
- `README-ja.md`: Description in Japanese
- `README-en.md`: Description in English (this file)

## Description
Nothing particularly.

## How to compile
Do `make` as shown in Section 9.1. Or, to only build the executable file, do
```
node00:~/snsbook/code/part2/cpg$ make cpg
```

## How to run
Follow the textbook. The following line in `cpg.c` should be changed:
```
#define W       (  -40.0 ) // connection weight
```
If this line remains the same, Figure 9.2b will be obtained. If the value is set at 0,
Figure 9.2a will be obtained.
