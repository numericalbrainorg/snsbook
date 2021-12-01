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
#define T_REFR  (    5.0 ) // ms; unused
#define NT_REFR (    5   ) // ( T_REFR / DT )

int main ( void )
{
  float v = V_REST;
  int32_t refr = 0; // counter for refractory period

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    float t = DT * nt;
    printf ( "%f %f\n", t, v );

    v += DT * ( - ( v - V_REST ) + R_M * I_EXT ) / TAU;
    bool s = ( v > THETA );

    // Pretty-print spikes on membrane potentials. Note that spike time is not t but t + DT
    if ( s ) { printf ( "%f %f\n%f %f\n", t + DT, v, t + DT, 0. ); }

    refr = s * ( NT_REFR ) + ( ! s ) * ( refr - 1 ); // set counter
    v = ( refr > 0 ) ? V_RESET : v;
  }
}
