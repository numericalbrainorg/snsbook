#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#define T  ( 1000  ) // unused
#define DT (    1. )
#define NT ( 1000  )

enum { L, R, N_PRE };
#define N_POST_X ( 32 )
#define N_POST_Y ( 32 )
#define N_POST ( ( N_POST_X ) * ( N_POST_Y ) )

// Neuron dynamics
#define TAU_M   (  20. )
#define V_REST  ( -49. )
#define V_INIT  ( -60. )
#define V_RESET ( -60. )
#define THETA   ( -50. )

// Synapse dynamics
#define G_AFF ( 2. )
#define G_LAT ( 2. )
#define TAU_AFF ( 5. )
#define TAU_LAT ( 5. )

// STDP
#define A_d ( 0.03 )
#define A_p ( 0.03 )
#define TAU_PRE  ( 10. )
#define TAU_POST ( 10. )

// Lateral connection
#define S1 (  4.0 )
#define S2 ( 13.0 )
#define K1 (  1.0 )
#define K2 ( ( S1 ) / ( S2 ) )

typedef struct {
  float *w_aff, *w_lat;
  float *v, *g_aff, *g_lat;
  float *trace_pre, *trace_post, *syn_aff, *syn_lat;
  bool *spike_pre, *spike_post;
} od_t;

void initialize ( sfmt_t *rng, od_t *od )
{
  sfmt_init_gen_rand ( rng, 42 );

  *od = ( od_t ) { . w_aff = calloc ( N_PRE  * N_POST, sizeof ( float ) ),
		   . w_lat = calloc ( N_POST * N_POST, sizeof ( float ) ),
		   . v = calloc ( N_POST, sizeof ( float ) ),
		   . g_aff = calloc ( N_POST, sizeof ( float ) ),
		   . g_lat = calloc ( N_POST, sizeof ( float ) ),
		   . trace_pre  = calloc ( N_PRE,  sizeof ( float ) ),
		   . trace_post = calloc ( N_POST, sizeof ( float ) ),
		   . syn_aff = calloc ( N_POST, sizeof ( float ) ),
		   . syn_lat = calloc ( N_POST, sizeof ( float ) ),
		   . spike_pre  = calloc ( N_PRE,  sizeof ( bool ) ),
		   . spike_post = calloc ( N_POST, sizeof ( bool ) ), };

  for ( int32_t i = 0; i < N_POST; i++ ) {
    for ( int32_t j = 0; j < N_PRE; j++ ) {
      od -> w_aff [ j + N_PRE * i ] = 0.5 + 0.1 * 0.5 * ( 1 - sfmt_genrand_real2 ( rng ) );
    }
  }

  for ( int32_t i = 0; i < N_POST; i++ ) { od -> v [ i ] = V_INIT + 10. * sfmt_genrand_real2 ( rng ); }

  float norm_coef = 1.0 / ( K1 * sqrt ( 2 * S1 * M_PI ) - K2 * sqrt ( 2 * S2 * M_PI ) );
  for ( int32_t i = 0; i < N_POST; i++ ) {
    for ( int32_t j = 0; j < N_POST; j++ ) {
      int32_t ix = i / N_POST_X;
      int32_t iy = i % N_POST_X;
      int32_t jx = j / N_POST_X;
      int32_t jy = j % N_POST_X;
      int32_t dx = abs ( ix - jx );
      int32_t dy = abs ( iy - jy );

      // Periodic boundary condition
      dx -= ( dx > N_POST_X / 2 ) ? N_POST_X / 2 : 0;
      dy -= ( dy > N_POST_Y / 2 ) ? N_POST_Y / 2 : 0;
      float d2 = dx * dx + dy * dy;

      od -> w_lat [ j + N_POST * i ] = ( norm_coef *
					 ( K1 * exp ( - d2 / ( 2. * S1 ) ) - K2 * exp ( - d2 / ( 2. * S2 ) ) ) );
    }
  }
}

void finalize ( od_t *od )
{
  free ( od -> w_aff );
  free ( od -> w_lat );
  free ( od -> v );
  free ( od -> g_aff );
  free ( od -> g_lat );
  free ( od -> trace_pre );
  free ( od -> trace_post );
  free ( od -> syn_aff );
  free ( od -> syn_lat );
  free ( od -> spike_pre );
  free ( od -> spike_post );
}

void calculateSynapticInputs ( sfmt_t *rng, od_t *od )
{
  float activity [ N_PRE ] = { 1.0, 1.0 };
  for ( int32_t i = 0; i < N_PRE; i++ ) {
    od -> spike_pre [ i ] = ( sfmt_genrand_real2 ( rng ) < 0.1 * activity [ i ] ) ? 1 : 0;
  }

  #pragma omp parallel for
  for ( int32_t i = 0; i < N_POST; i++ ) {
    float r = 0.;
    for ( int32_t j = 0; j < N_PRE; j++ ) { r += od -> w_aff [ j + N_PRE * i ] * od -> spike_pre [ j ]; }
    od -> g_aff [ i ] = exp ( - DT / TAU_AFF ) * od -> g_aff [ i ] + r;
  }

  #pragma omp parallel for
  for ( int32_t i = 0; i < N_POST; i++ ) {
    float r = 0.;
    for ( int32_t j = 0; j < N_POST; j++ ) { r += od -> w_lat [ j + N_POST * i ] * od -> spike_post [ j ]; }
    od -> g_lat [ i ] = exp ( - DT / TAU_LAT ) * od -> g_lat [ i ] + r;
  }
}

void updateCellParameters ( od_t *od )
{
  #pragma omp parallel for
  for ( int32_t i = 0; i < N_POST; i++ ) {
    od -> v [ i ] += DT * ( - ( od -> v [ i ] - V_REST ) + G_AFF * od -> g_aff [ i ] + G_LAT * od -> g_lat [ i ] ) / TAU_M;
    od -> spike_post [ i ] = ( od -> v [ i ] > THETA );
    od -> v [ i ] = od -> spike_post [ i ] * V_RESET + ( ! od -> spike_post [ i ] ) * ( od -> v [ i ] );
  }
}

void updateSynapticWeights ( od_t *od )
{
  for ( int32_t i = 0; i < N_PRE; i++ ) {
    od -> trace_pre [ i ] = exp ( - DT / TAU_PRE ) * od -> trace_pre [ i ] + od -> spike_pre [ i ];
  }

  for ( int32_t i = 0; i < N_POST; i++ ) {
    od -> trace_post [ i ] = exp ( - DT / TAU_POST ) * od -> trace_post [ i ] + od -> spike_post [ i ];
  }

  #pragma omp parallel for
  for ( int32_t i = 0; i < N_POST; i++ ) {
    for ( int32_t j = 0; j < N_PRE; j++ ) {
      int32_t k = j + N_PRE * i;
      od -> w_aff [ k ] += - A_d * od -> spike_pre [ j ] * od -> trace_post [ i ] + A_p * od -> trace_pre [ j ] * od -> spike_post [ i ];
      od -> w_aff [ k ] =  ( od -> w_aff [ k ] < 0. ) ? 0. : od -> w_aff [ k ];
    }
  }
}

void outputOD ( const char *fname, const float *w_aff )
{
  float odmax = 0.;
  int32_t xmax = 0, ymax = 0;
  FILE *file = fopen ( fname, "w" );
  for ( int32_t x = 0; x < N_POST_X; x++ ) {
    for ( int32_t y = 0; y < N_POST_Y; y++ ) {
      int32_t i = y + N_POST_Y * x;
      float od = w_aff [ L + N_PRE * i ] - w_aff [ R + N_PRE * i ];
      fprintf ( file, "%d %d %f\n", x, y, od );
      if ( fabs ( od ) > odmax ) {
	xmax = x;
	ymax = y;
	odmax = fabs ( od );
      }
    }
    fprintf ( file, "\n" );
  }
  fprintf ( file, "# %d %d %f\n", xmax, ymax, odmax );
  fclose ( file );
}

int main ( void )
{
  sfmt_t rng;
  od_t od;

  initialize ( &rng, &od );

  outputOD ( "before.dat", od . w_aff );

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    calculateSynapticInputs ( &rng, &od );
    updateCellParameters ( &od );
    updateSynapticWeights ( &od );
  }

  outputOD ( "after.dat", od . w_aff );

  finalize ( &od );
}
