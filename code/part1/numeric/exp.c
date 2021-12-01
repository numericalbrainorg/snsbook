#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>

static inline double f ( const double x, const double t ) { return x; }

double euler ( const double x, const double t, const double dt )
{
  double k1 = dt * f ( x, t );
  return x + k1;
}

double heun ( const double x, const double t, const double dt )
{
  double k1 = dt * f ( x, t );
  double k2 = dt * f ( x + k1, t + dt ); 
  return x + .5 * ( k1 + k2 );
}

double rk4 ( const double x, const double t, const double dt )
{
  double k1 = dt * f ( x, t );
  double k2 = dt * f ( x + .5 * k1, t + .5 * dt ); 
  double k3 = dt * f ( x + .5 * k2, t + .5 * dt );
  double k4 = dt * f ( x + k3, t + dt ); 
  return x + ( k1 + 2 * k2 + 2 * k3 + k4 ) / 6.;
}

int main ( void )
{
  double dt = 1.;
  while ( 1 ) {
    double e = 1., h = 1., r = 1.;
    for (int32_t i = 0; i < floor (1. / dt ); i++ ) {
      double t = i * dt;
      e = euler ( e, t, dt );
      h = heun  ( h, t, dt );
      r = rk4   ( r, t, dt );
    }
    printf ( "%1.15f %1.15f %1.15f %1.15f %1.1g %1.1g %1.1g\n",
	     dt, e, h, r, fabs ( e - exp ( 1. ) ) , fabs ( h - exp ( 1 ) ), fabs ( r - exp ( 1 ) ) );
    if ( fabs ( r - exp ( 1 ) ) < 1e-14 ) { break; }
    dt /= 2;
  }

  printf ( "%1.15f\n", exp ( 1 ) );
}
