#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

#define C      (   1.0 ) // micro F / cm^2
#define G_LEAK (   0.3 ) // mS / cm^2
#define E_LEAK ( -17.0 ) // mV
#define G_NA   ( 120.0 ) // mS / cm^2
#define E_NA   (  55.0 ) // mV
#define G_K    (  20.0 ) // mS / cm^2
#define E_K    ( -75.0 ) // mV
#define G_A    (  47.7 ) // mS / cm^2
#define E_A    ( -75.0 ) // mV

#define DT ( 0.01 ) // 10 micro s
#define T  ( 1000 ) // 1000 ms; unused
#define NT ( 100000 ) // T / DT

static inline double alpha_m ( const double v ) { const double mshft = -5.3; return ( - 0.1 * ( v + 35. + mshft) ) / ( exp (- 0.1 * ( v + 35. + mshft ) ) - 1.0 ); }
static inline double beta_m  ( const double v ) { const double mshft = -5.3; return 4.0 * exp ( - ( v + 60. + mshft ) / 18.0 ); }
static inline double alpha_h ( const double v ) { const double hshft = -12.; return 0.07 * exp ( - ( v + 60. + hshft) / 20.0 ); }
static inline double beta_h  ( const double v ) { const double hshft = -12.; return 1.0 / ( exp ( - 0.1 * ( v  + 30. + hshft ) ) + 1.0 ); }
static inline double alpha_n ( const double v ) { const double nshft = -4.3; return ( - 0.01 * ( v + 50. + nshft ) ) / ( exp ( - 0.1 * ( v + 50. + nshft ) ) - 1.0 ); }
static inline double beta_n  ( const double v ) { const double nshft = -4.3; return 0.125 * exp ( - ( v + 60 + nshft ) / 80.0 ); }

static inline double m0 ( const double v ) { return alpha_m ( v ) / ( alpha_m ( v ) + beta_m ( v ) ); }
static inline double h0 ( const double v ) { return alpha_h ( v ) / ( alpha_h ( v ) + beta_h ( v ) ); }
static inline double n0 ( const double v ) { return alpha_n ( v ) / ( alpha_n ( v ) + beta_n ( v ) ); }
static inline double a0 ( const double v ) { return pow ( 0.0761 * exp ( ( v + 94.22 ) / 31.84 ) /
					    ( 1. + exp ( ( v + 1.17 ) / 28.93 ) ), 0.33333 ); }
static inline double b0 ( const double v ) { return 1. / pow ( 1 + exp ( ( v + 53.3 ) / 14.54 ), 4 ) ; }
static inline double tau_a ( const double v ) { return 0.3632 + 1.158 / ( 1. + exp ( ( v + 55.96 ) / 20.12 ) ); }
static inline double tau_b ( const double v ) { return 1.24   + 2.678 / ( 1. + exp ( ( v + 50 ) / 16.027 ) ) ; }
static inline double tau_m ( const double v ) { return ( 1. / 3.8 ) * ( 1. / ( alpha_m ( v ) + beta_m ( v ) ) ); }
static inline double tau_h ( const double v ) { return ( 1. / 3.8 ) * ( 1. / ( alpha_h ( v ) + beta_h ( v ) ) ); }
static inline double tau_n ( const double v ) { return ( 2. ) * ( 1. / ( alpha_n ( v ) + beta_n ( v ) ) ); }

static inline double dmdt ( const double v, const double m ) { return ( 1.0 / tau_m ( v ) ) * ( - m + m0 ( v ) ); }
static inline double dhdt ( const double v, const double h ) { return ( 1.0 / tau_h ( v ) ) * ( - h + h0 ( v ) ); }
static inline double dndt ( const double v, const double n ) { return ( 1.0 / tau_n ( v ) ) * ( - n + n0 ( v ) ); }
static inline double dadt ( const double v, const double a ) { return ( 1.0 / tau_a ( v ) ) * ( - a + a0 ( v ) ); }
static inline double dbdt ( const double v, const double b ) { return ( 1.0 / tau_b ( v ) ) * ( - b + b0 ( v ) ); }
static inline double dvdt ( const double v, const double m, const double h, const double n, const double a, const double b, const double I_ext )
{
  return ( - G_LEAK * ( v - E_LEAK ) - G_NA * m * m * m * h * ( v - E_NA ) - G_K * n * n * n * n * ( v - E_K )
	   - G_A * a * a * a * b * ( v - E_A ) // I_A-current
	   + I_ext ) / C;
}

int main ( void )
{
  double v = E_LEAK;
  double m = m0 ( v );
  double h = h0 ( v );
  double n = n0 ( v );
  double a = a0 ( v );
  double b = b0 ( v );

  //double I_ext = 9.0; // micro A / cm^2
  double I_ext = 8.60941453; // micro A / cm^2

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    double t = DT * nt;
    printf ( "%f %f\n", t, v );

    double dmdt1 = dmdt ( v, m );
    double dhdt1 = dhdt ( v, h );
    double dndt1 = dndt ( v, n );
    double dadt1 = dadt ( v, a );
    double dbdt1 = dbdt ( v, b );
    double dvdt1 = dvdt ( v, m, h, n, a, b, I_ext ); 

    double dmdt2 = dmdt ( v + .5 * DT * dvdt1, m + .5 * DT * dmdt1 );
    double dhdt2 = dhdt ( v + .5 * DT * dvdt1, h + .5 * DT * dhdt1 );
    double dndt2 = dndt ( v + .5 * DT * dvdt1, n + .5 * DT * dndt1 );
    double dadt2 = dadt ( v + .5 * DT * dvdt1, a + .5 * DT * dadt1 );
    double dbdt2 = dbdt ( v + .5 * DT * dvdt1, b + .5 * DT * dbdt1 );
    double dvdt2 = dvdt ( v + .5 * DT * dvdt1, m + .5 * DT * dmdt1, h + .5 * DT * dhdt1, n + .5 * DT * dndt1,
			  a + .5 * DT * dadt1, b + .5 * DT * dbdt1, I_ext );

    double dmdt3 = dmdt ( v + .5 * DT * dvdt2, m + .5 * DT * dmdt2 );
    double dhdt3 = dhdt ( v + .5 * DT * dvdt2, h + .5 * DT * dhdt2 );
    double dndt3 = dndt ( v + .5 * DT * dvdt2, n + .5 * DT * dndt2 );
    double dadt3 = dadt ( v + .5 * DT * dvdt2, a + .5 * DT * dadt2 );
    double dbdt3 = dbdt ( v + .5 * DT * dvdt2, b + .5 * DT * dbdt2 );
    double dvdt3 = dvdt ( v + .5 * DT * dvdt2, m + .5 * DT * dmdt2, h + .5 * DT * dhdt2, n + .5 * DT * dndt2,
			  a + .5 * DT * dadt2, b + .5 * DT * dbdt2, I_ext );

    double dmdt4 = dmdt ( v + DT * dvdt3, m + DT * dmdt3 );
    double dhdt4 = dhdt ( v + DT * dvdt3, h + DT * dhdt3 );
    double dndt4 = dndt ( v + DT * dvdt3, n + DT * dndt3 );
    double dadt4 = dadt ( v + DT * dvdt3, a + DT * dadt3 );
    double dbdt4 = dbdt ( v + DT * dvdt3, b + DT * dbdt3 );
    double dvdt4 = dvdt ( v + DT * dvdt3, m + DT * dmdt3, h + DT * dhdt3, n + DT * dndt3,
			  a + DT * dadt3, b + DT * dbdt3, I_ext );

    m += DT * ( dmdt1 + 2 * dmdt2 + 2 * dmdt3 + dmdt4 ) / 6.;
    h += DT * ( dhdt1 + 2 * dhdt2 + 2 * dhdt3 + dhdt4 ) / 6.;
    n += DT * ( dndt1 + 2 * dndt2 + 2 * dndt3 + dndt4 ) / 6.;
    a += DT * ( dadt1 + 2 * dadt2 + 2 * dadt3 + dadt4 ) / 6.;
    b += DT * ( dbdt1 + 2 * dbdt2 + 2 * dbdt3 + dbdt4 ) / 6.;
    v += DT * ( dvdt1 + 2 * dvdt2 + 2 * dvdt3 + dvdt4 ) / 6.;
  }
}
