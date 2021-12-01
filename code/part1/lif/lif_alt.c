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

int main ( void )
{
  float v = V_REST;

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    float t = DT * nt;
    printf ( "%f %f\n", t, v );

    bool s = ( v > THETA );
    float dv = DT * ( - ( v - V_REST ) + R_M * I_EXT ) / TAU;

    v = s * V_RESET + ( ! s ) * ( v + dv );
  }
}
