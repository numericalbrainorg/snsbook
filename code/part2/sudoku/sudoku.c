#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#define NN ( 2 )
#define N  ( ( NN ) * ( NN ) ) // size of a board

int32_t puzzle [ N ] [ N ] = { {  2,  3,  0,  1 },
			       { -1, -1, -1, -1 },
			       { -1, -1, -1, -1 },
			       {  3,  1,  2,  0 } };

#define N_NEURON ( ( N ) * ( N ) * ( N ) )
#define TAU    (  10. )
#define W_INH  ( -10. )
#define E_REST ( -65. )
#define THETA  ( -55. )
#define I_EXT  (  20. )
#define DECAY  (  0.5 )

#define NT ( 1000 )
#define DT ( 1.0 )

static inline int32_t id  ( const int32_t i, const int32_t j, const int32_t a ) { return ( a + N * ( j + N * ( i ) ) ); }
static inline int32_t wid ( const int32_t post, const int32_t pre ) { return ( pre + N_NEURON * post ); }

void createConnection ( float * w )
{
  for ( int32_t i = 0; i < N; i++ ) { // row
    for ( int32_t j = 0; j < N; j++ ) { // col
      for ( int32_t a = 0; a < N; a++ ) { // neuron
	int32_t post = id ( i, j, a );
	// grid
	for ( int32_t b = 0; b < N; b++ ) { // neuron
	  int32_t pre = id ( i, j, b );
	  int32_t x = wid ( post, pre );
	  w [ x ] = ( a != b ) ? W_INH : 0.;
	}
	// row
	for ( int32_t l = 0; l < N; l++ ) { // row
	  for ( int32_t b = 0; b < N; b++ ) { // neuron
	    int32_t pre = id ( l, j, b );
	    int32_t x = wid ( post, pre );
	    w [ x ] = ( i != l && a == b ) ? W_INH : 0.;
	  }
	}
	// column
	for ( int32_t l = 0; l < N; l++ ) { // col
	  for ( int32_t b = 0; b < N; b++ ) { // neuron
	    int32_t pre = id ( i, l, b );
	    int32_t x = wid ( post, pre );
	    w [ x ] = ( j != l && a == b ) ? W_INH : 0.;
	  }
	}
	// box
	for ( int32_t ll = 0; ll < NN; ll++ ) { // row2
	  for ( int32_t mm = 0; mm < NN; mm++ ) { // col2
	    int32_t ib = i / NN;
	    int32_t jb = j / NN;
	    int32_t l = ll + NN * ib;
	    int32_t m = mm + NN * jb;
	    for ( int32_t b = 0; b < N; b++ ) { // neuron
	      int32_t pre = id ( l, m, b );
	      int32_t x = wid ( post, pre );
	      w [ x ] = ( !( i == l && j == m ) && a == b ) ? W_INH : 0.;
	    }
	  }
	}
      }
    }
  }
}

void setInput ( float * i_ext )
{
  for ( int32_t i = 0; i < N; i++ ) {
    for ( int32_t j = 0; j < N; j++ ) {
      int32_t b = puzzle [ i ] [ j ];
      if ( b >= 0 ) {
	for ( int32_t a = 0; a < N; a++ ) {
	  int32_t x = id ( i, j, a );
	  i_ext [ x ] = ( a == b ) ? 1 : 0;
	}
      }
    }
  }
}

void printAnswer ( const int32_t *ns )
{
  for ( int32_t i = 0; i < N; i++ ) {
    for ( int32_t j = 0; j < N; j++ ) {
      int32_t maxa = 0;
      int32_t maxs = ns [ id ( i, j, maxa ) ];
      for ( int32_t a = 0; a < N; a++ ) {
	if ( maxs < ns [ id ( i, j, a ) ] ) {
	  maxa = a;
	  maxs = ns [ id ( i, j, maxa ) ];
	}
      }
      fprintf ( stderr, "%d%s", maxa, (j == N - 1 ) ? "\n" : " " );
    }
  }
}

int main ( void )
{
  float *w = ( float * ) calloc ( N_NEURON * N_NEURON, sizeof ( float ) );
  createConnection ( w );

  float *i_ext = ( float * ) calloc ( N_NEURON, sizeof ( float ) );
  setInput ( i_ext );

  float *v = ( float * ) calloc ( N_NEURON, sizeof ( float ) );
  for ( int32_t i = 0; i < N_NEURON; i++ ) { v [ i ] = E_REST; }

  float *g_syn = ( float * ) calloc ( N_NEURON, sizeof ( float ) );
  bool *s = ( bool * ) calloc ( N_NEURON, sizeof ( bool ) ); // spike 0 / 1
  int32_t *ns = ( int32_t * ) calloc ( N_NEURON, sizeof ( int32_t ) ); // # spikes emitted

  sfmt_t rng;
  sfmt_init_gen_rand ( &rng, getpid () );

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    for ( int32_t i = 0; i < N_NEURON; i++ ) {
      float r = 0;
      for ( int32_t j = 0; j < N_NEURON; j++ ) {
	int32_t x = wid ( i, j );
	r += w [ x ] * s [ j ];
      }
      g_syn [ i ] = DECAY * g_syn [ i ] + r;
    }
    for ( int32_t i = 0; i < N_NEURON; i++ ) {
      float dv = DT * ( - ( v [ i ] - E_REST ) + g_syn [ i ] + I_EXT * ( i_ext [ i ] + sfmt_genrand_real2 ( &rng ) ) ) / TAU;
      s [ i ] = ( v [ i ] > THETA );
      v [ i ] = s [ i ] * ( E_REST ) + ( ! s [ i ] ) * ( v [ i ] + dv );
      if ( nt >= NT / 2 ) { ns [ i ] += s [ i ]; }
      // if ( s [ i ] ) { printf ( "%d %d\n", nt, i ); } // print spikes for raster plots
    }
  }

  printAnswer ( ns );

  free ( v );
  free ( s );
  free ( i_ext );
  free ( w );
}
