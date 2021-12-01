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
#define TAU_SYN (    5.0 ) // ms
#define R_SYN   (    1.0 ) // MOhm
#define W       (    2.0 ) // connection weight
#define T_REFR  (    5.0 ) // ms; unused
#define NT_REFR (    5   ) // ( T_REFR / DT )
#define DELAY_SYN  ( 2.0 ) // ms; unused
#define NDELAY_SYN ( 2   ) // ( DELAY_SYN / DT )

int main ( void )
{
  float v [ N ] = { V_REST, V_REST - 15. };
  float i_syn [ N ] = { 0., 0. };
  bool s [ N ] = { false, false };
  int32_t ts [ N ] = { 0, 0 };
  int32_t refr [ N ] = { 0, 0 };

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    float t = DT * nt;
    printf ( "%f %f %f\n", t, v [ 0 ], v [ 1 ] );

    for ( int32_t i = 0; i < N; i++ ) { i_syn [ i ] = exp ( - DT / TAU_SYN ) * i_syn [ i ] + W * ( ts [ ( i + 1 ) % 2 ] + NDELAY_SYN == nt ); }
    for ( int32_t i = 0; i < N; i++ ) {
      v [ i ] += DT * ( - ( v [ i ] - V_REST ) + R_SYN * i_syn [ i ] + R_M * I_EXT ) / TAU;
      s [ i ] = ( v [ i ] > THETA );
      ts [ i ] = ( s [ i ] ) * ( nt + 1 ) + ( ! s [ i ] ) * ts [ i ];
    }

    // Pretty-print spikes on membrane potentials. Note that spike time is not t but t + DT
    if ( s [ 0 ] ) { printf ( "%f %f %f\n%f %f %f\n", t + DT, v [ 0 ], v [ 1 ], t + DT, 0., v [ 1 ] ); }
    if ( s [ 1 ] ) { printf ( "%f %f %f\n%f %f %f\n", t + DT, v [ 0 ], v [ 1 ], t + DT, v [ 0 ], 0. ); }

    for ( int32_t i = 0; i < N; i++ ) {
      refr [ i ] = s [ i ] * ( NT_REFR ) + ( ! s [ i ] ) * ( refr [ i ] - 1 );
      v [ i ] = ( refr [ i ] > 0 ) ? V_RESET : v [ i ];
    }
  }
}
