#pragma once

#define C      (   1.0 ) // micro F/cm^2
#define G_LEAK (   0.3 ) // mS / cm^2
#define E_LEAK (  10.6 ) // mV
#define G_NA   ( 120.0 ) // mS / cm^2
#define E_NA   ( 115.0 ) // mV
#define G_K    (  36.0 ) // mS / cm^2
#define E_K    ( -12.0 ) // mV

extern double alpha_m ( const double );
extern double beta_m  ( const double );
extern double alpha_h ( const double );
extern double beta_h  ( const double );
extern double alpha_n ( const double );
extern double beta_n  ( const double );

extern double inf_m ( const double );
extern double inf_h ( const double );
extern double inf_n ( const double );
