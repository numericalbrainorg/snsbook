#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#define N       ( 4000 )            // # of neurons 
#define N_E     ( 3200 )            // # of excitatory neurons
#define N_I     ( ( N ) - ( N_E ) ) // # of inhibitory neurons 

#define T       ( 1000 ) // total simulation time ( 1000 ms, unused )
#define DT      ( 1.   ) // delta t = 1 ms
#define NT      ( 1000 ) // # of steps ( = T / DT )

#define TAU_M   (  20. ) // membrane time constant ( ms )
#define TAU_E   (  5.  ) // excitatory synapse time constant ( ms )
#define TAU_I   (  10. ) // inhibitory synapse time constant ( ms )

#define V_REST  ( -49. ) // resting potential ( mV )
#define V_INIT  ( -60. ) // initial value ( mV )
#define V_RESET ( -60. ) // reset potential ( mV )
#define THETA   ( -50. ) // threshold ( mV )

#define G_E     ( 1.62 / ( TAU_E ) ) // 1 / peak excitatory synaptic conductance
#define G_I     ( -9.  / ( TAU_I ) ) // 1 / peak inhibitory synaptic conductance
#define P       ( 0.02 ) // probability of making synaptic connections

typedef struct {
  float *v, *ge, *gi, *w;
  bool *s;
  sfmt_t rng;
  FILE *file;
} network_t;

extern void timer_start ( void );
extern double timer_elapsed ( void );

void initialize ( network_t *n )
{
  *n = ( network_t ) { . v  = calloc ( N, sizeof ( float ) ),
		       . ge = calloc ( N, sizeof ( float ) ),
		       . gi = calloc ( N, sizeof ( float ) ),
		       . w  = calloc ( N * N, sizeof ( float ) ),
		       . s  = calloc ( N, sizeof ( bool ) ),
		       . file = fopen ( "spike.dat", "w"), };

  // PRNG
  sfmt_init_gen_rand ( &n -> rng, 23 );

  // Membrane potentials
  for ( int32_t i = 0; i < N; i++ ) {
    n -> v [ i ] = V_INIT + 10. * sfmt_genrand_real2 ( &n -> rng );
  }

  // Synaptic connections
  for ( int32_t i = 0; i < N; i++ ) {
    for ( int32_t j = 0; j < N; j++ ) {
      n -> w [ j + N * i ] = ( sfmt_genrand_real2 ( &n -> rng ) < P ) ? 1. : 0.;
    }
  }
}

void finalize ( network_t *n )
{
  free ( n -> v );
  free ( n -> ge );
  free ( n -> gi );
  free ( n -> w );
  free ( n -> s );
  fclose ( n -> file );
}

void calculateSynapticInputs ( network_t *n )
{
  for ( int32_t i = 0; i < N; i++ ) {
    float re = 0, ri = 0;
    for ( int32_t j = 0; j < N; j++ ){
      float r = n -> w [ j + N * i ] * n -> s [ j ];
      if ( j < N_E ) { re += r; } else { ri += r; }
    }
    n -> ge [ i ] = exp ( - DT / TAU_E ) * n -> ge [ i ] + re;
    n -> gi [ i ] = exp ( - DT / TAU_I ) * n -> gi [ i ] + ri;
  }
}

void updateCellParameters ( network_t *n )
{
  for ( int32_t i = 0; i < N; i++ ) {
    n -> v [ i ] += DT * ( - ( n -> v [ i ] - V_REST ) + G_E * n -> ge [ i ] + G_I * n -> gi [ i ] ) / TAU_M;
    n -> s [ i ] = ( n -> v [ i ] > THETA );
    n -> v [ i ] = ( n -> s [ i ] ) * V_RESET + ( ! n -> s [ i ] ) * n -> v [ i ];
  }
}

void outputSpike ( const int32_t nt, network_t *n )
{
  for ( int32_t i = 0; i < N; i++ ) {
    if ( n -> s [ i ] ) { fprintf ( n -> file, "%f %d\n", DT * ( nt + 1 ), i ); } // Spike time is not t but t + DT
  }
}

void loop ( network_t *n )
{
  timer_start ( );

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    calculateSynapticInputs ( n );
    updateCellParameters ( n );
    outputSpike ( nt, n );
  }

  double elapsedTime = timer_elapsed ( );
  printf ( "Elapsed time = %f sec.\n", elapsedTime);
}

int main ( void )
{
  network_t n;

  initialize ( &n );
  loop ( &n );
  finalize ( &n );
}
