#include<stdio.h>
#include<stdlib.h>
#include<math.h>

#ifndef HEADER_H
#define HEADER_H


#define dt 0.00001
#define T 10.0
#define NOS (12+1) // number of oscillators
#define NMS (14+1) // dimension of M-S vector
#define NCF (8+1) // dimension of M-S vector
#define NTR (6+1) // number of torques

enum { XR, YR, XL, YL };

extern void os_initialize(const char *, const char *);
extern void os_finalize(void);
extern void os_get_y(double []);
extern void os_compute(const double, const double []);
extern void os_update(const double);
extern void os_print(const double, const double []);

extern void ms_initialize(const char *);
extern void ms_finalize(void);
extern void ms_get_feed(double []);
extern void ms_get_anklepos(double[]);
extern void ms_compute(const double, const double []);
extern void ms_update(const double);
extern void ms_print(const double, const double []);

#endif
