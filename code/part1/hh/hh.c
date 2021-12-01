#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define E_REST ( -65.0 ) // mV
#define C      (   1.0 ) // micro F / cm^2
#define G_LEAK (   0.3 ) // mS / cm^2
#define E_LEAK (  10.6 + ( E_REST ) ) // mV
#define G_NA   ( 120.0 ) // mS / cm^2
#define E_NA   ( 115.0 + ( E_REST ) ) // mV
#define G_K    (  36.0 ) // mS / cm^2
#define E_K    ( -12.0 + ( E_REST ) ) // mV

#define DT ( 0.01 ) // 10 micro s
#define T  ( 1000 ) // 1000 ms; unused
#define NT ( 100000 ) // T / DT

static inline double alpha_m ( const double v ) { return ( 2.5 - 0.1 * ( v - E_REST ) ) / ( exp ( 2.5 - 0.1 * ( v - E_REST ) ) - 1.0 ); }
static inline double beta_m  ( const double v ) { return 4.0 * exp ( - ( v - E_REST ) / 18.0 ); }
static inline double alpha_h ( const double v ) { return 0.07 * exp ( - ( v - E_REST ) / 20.0 ); }
static inline double beta_h  ( const double v ) { return 1.0 / ( exp ( 3.0 - 0.1 * ( v - E_REST ) ) + 1.0 ); }
static inline double alpha_n ( const double v ) { return ( 0.1 - 0.01 * ( v - E_REST ) ) / ( exp ( 1 - 0.1 * ( v - E_REST ) ) - 1.0 ); }
static inline double beta_n  ( const double v ) { return 0.125 * exp ( - ( v - E_REST ) / 80.0 ); }

static inline double m0 ( const double v ) { return alpha_m ( v ) / ( alpha_m ( v ) + beta_m ( v ) ); }
static inline double h0 ( const double v ) { return alpha_h ( v ) / ( alpha_h ( v ) + beta_h ( v ) ); }
static inline double n0 ( const double v ) { return alpha_n ( v ) / ( alpha_n ( v ) + beta_n ( v ) ); }
static inline double tau_m ( const double v ) { return 1. / ( alpha_m ( v ) + beta_m ( v ) ); }
static inline double tau_h ( const double v ) { return 1. / ( alpha_h ( v ) + beta_h ( v ) ); }
static inline double tau_n ( const double v ) { return 1. / ( alpha_n ( v ) + beta_n ( v ) ); }

static inline double dmdt ( const double v, const double m ) { return ( 1.0 / tau_m ( v ) ) * ( - m + m0 ( v ) ); }
static inline double dhdt ( const double v, const double h ) { return ( 1.0 / tau_h ( v ) ) * ( - h + h0 ( v ) ); }
static inline double dndt ( const double v, const double n ) { return ( 1.0 / tau_n ( v ) ) * ( - n + n0 ( v ) ); }
static inline double dvdt ( const double v, const double m, const double h, const double n, const double i_ext )
{
  return ( - G_LEAK * ( v - E_LEAK ) - G_NA * m * m * m * h * ( v - E_NA ) - G_K * n * n * n * n * ( v - E_K ) + i_ext ) / C;
}

int main ( void )
{
  double v = E_REST;
  double m = m0 ( v );
  double h = h0 ( v );
  double n = n0 ( v );

  double i_ext = 9.0; // micro A / cm^2

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    double t = DT * nt;
    printf ( "%f %f %f %f %f\n", t, v, m, h, n );

    double dmdt1 = dmdt ( v, m );
    double dhdt1 = dhdt ( v, h );
    double dndt1 = dndt ( v, n );
    double dvdt1 = dvdt ( v, m, h, n, i_ext ); 

    double dmdt2 = dmdt ( v + .5 * DT * dvdt1, m + .5 * DT * dmdt1 );
    double dhdt2 = dhdt ( v + .5 * DT * dvdt1, h + .5 * DT * dhdt1 );
    double dndt2 = dndt ( v + .5 * DT * dvdt1, n + .5 * DT * dndt1 );
    double dvdt2 = dvdt ( v + .5 * DT * dvdt1, m + .5 * DT * dmdt1, h + .5 * DT * dhdt1, n + .5 * DT * dndt1, i_ext ); 

    double dmdt3 = dmdt ( v + .5 * DT * dvdt2, m + .5 * DT * dmdt2 );
    double dhdt3 = dhdt ( v + .5 * DT * dvdt2, h + .5 * DT * dhdt2 );
    double dndt3 = dndt ( v + .5 * DT * dvdt2, n + .5 * DT * dndt2 );
    double dvdt3 = dvdt ( v + .5 * DT * dvdt2, m + .5 * DT * dmdt2, h + .5 * DT * dhdt2, n + .5 * DT * dndt2, i_ext ); 

    double dmdt4 = dmdt ( v + DT * dvdt3, m + DT * dmdt3 );
    double dhdt4 = dhdt ( v + DT * dvdt3, h + DT * dhdt3 );
    double dndt4 = dndt ( v + DT * dvdt3, n + DT * dndt3 );
    double dvdt4 = dvdt ( v + DT * dvdt3, m + DT * dmdt3, h + DT * dhdt3, n + DT * dndt3, i_ext ); 

    m += DT * ( dmdt1 + 2 * dmdt2 + 2 * dmdt3 + dmdt4 ) / 6.;
    h += DT * ( dhdt1 + 2 * dhdt2 + 2 * dhdt3 + dhdt4 ) / 6.;
    n += DT * ( dndt1 + 2 * dndt2 + 2 * dndt3 + dndt4 ) / 6.;
    v += DT * ( dvdt1 + 2 * dvdt2 + 2 * dvdt3 + dvdt4 ) / 6.;
  }
}
