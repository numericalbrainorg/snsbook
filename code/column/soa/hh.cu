#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hh.h"

double alpha_m ( const double v ) { return ( 2.5 - 0.1 * v ) / ( exp ( 2.5 - 0.1 * v ) - 1.0 ); }
double beta_m  ( const double v ) { return 4.0 * exp ( - v / 18.0 ); }
double alpha_h ( const double v ) { return 0.07 * exp ( - v / 20.0 ); }
double beta_h  ( const double v ) { return 1.0 / ( exp ( 3.0 - 0.1 * v ) + 1.0 ); }
double alpha_n ( const double v ) { return ( 0.1 - 0.01 * v ) / ( exp ( 1 - 0.1 * v ) - 1.0 ); }
double beta_n  ( const double v ) { return 0.125 * exp ( - v / 80.0 ); }

double inf_m ( const double v ) { return alpha_m ( v ) / ( alpha_m ( v ) + beta_m ( v ) ); }
double inf_h ( const double v ) { return alpha_h ( v ) / ( alpha_h ( v ) + beta_h ( v ) ); }
double inf_n ( const double v ) { return alpha_n ( v ) / ( alpha_n ( v ) + beta_n ( v ) ); }
