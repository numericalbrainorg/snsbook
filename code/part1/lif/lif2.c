#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#define TAU     (   20.0 ) // ms
#define V_REST  (  -65.0 ) // mV
#define V_RESET (  -65.0 ) // mV
#define THETA   (  -55.0 ) // mV
#define R_M     (    1.0 ) // MOhm
#define DT      (    1.0 ) // ms
#define T       ( 1000.0 ) // ms; unused
#define NT      ( 1000   ) // ( T / DT )
#define I_EXT   (   12.0 ) // nA
#define N       (    2   ) // # of neurons

int main ( void )
{
  float v [ N ] = { V_REST, V_REST - 15. };
  bool s [ N ] = { false, false };

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    float t = DT * nt;
    printf ( "%f %f %f\n", t, v [ 0 ], v [ 1 ] );

    for ( int32_t i = 0; i < N; i++ ) {
      v [ i ] += DT * ( - ( v [ i ] - V_REST ) + R_M * I_EXT ) / TAU;
      s [ i ] = ( v [ i ] > THETA );
    }

    // Pretty-print spikes on membrane potentials. Note that spike time is not t but t + DT
    if ( s [ 0 ] ) { printf ( "%f %f %f\n%f %f %f\n", t + DT, v [ 0 ], v [ 1 ], t + DT, 0., v [ 1 ] ); }
    if ( s [ 1 ] ) { printf ( "%f %f %f\n%f %f %f\n", t + DT, v [ 0 ], v [ 1 ], t + DT, v [ 0 ], 0. ); }

    for ( int32_t i = 0; i < N; i++ ) { v [ i ] = s [ i ] * V_RESET + ( ! s [ i ] ) * v [ i ]; }
  }
}
