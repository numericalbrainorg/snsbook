#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <unistd.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#define NX ( 5 )
#define NY ( 5 )
#define N  ( (NX) * (NY) )

int32_t p1 [ NX ] [ NY ] = {  { 0, 0, 1, 0, 0 },
			      { 0, 0, 1, 0, 0 },
			      { 1, 1, 1, 1, 1 },
			      { 0, 0, 1, 0, 0 },
			      { 0, 0, 1, 0, 0 }, };
int32_t p2 [ NX ] [ NY ] = {  { 1, 0, 0, 0, 1 },
			      { 0, 1, 0, 1, 0 },
			      { 0, 0, 1, 0, 0 },
			      { 0, 1, 0, 1, 0 },
			      { 1, 0, 0, 0, 1 }, };
const int32_t sort_p1 [ ] = { 2, 7, 10, 11, 12, 13, 14, 17, 22, 0, 1, 3, 4, 5, 6, 8, 9, 15, 16, 18, 19, 20, 21, 23, 24 };
const int32_t sort_p2 [ ] = { 0, 4, 6, 8, 12, 16, 18, 20, 24, 1, 2, 3, 5, 7, 9, 10, 11, 13, 14, 15, 17, 19, 21, 22, 23 };

#define TAU    (  20. )
#define W_EXC  (   5. )
#define E_REST ( -65. )
#define THETA  ( -55. )
#define R_M    (  16. )
#define I_EXT  (  1.5 )
#define DECAY  (  0.5 )

#define NT ( 1000 )
#define DT ( 1.0 )

static inline int32_t wid ( const int32_t post, const int32_t pre ) { return ( pre + N * post ); }

void createConnection ( float * w )
{
  for ( int32_t i = 0; i < N; i++ ) {
    for ( int32_t j = 0; j < N; j++ ) {
      w [ wid ( i, j ) ] = 0.;
    }
  }

  for ( int32_t i = 0; i < N; i++ ) {
    int32_t ix = i / NY;
    int32_t iy = i % NY;
    for ( int32_t j = 0; j < N; j++ ) {
      int32_t jx = j / NY;
      int32_t jy = j % NY;
      if ( i != j ) {
	w [ wid ( i, j ) ] += W_EXC * p1 [ ix ] [ iy ] * p1 [ jx ] [ jy ];
	w [ wid ( i, j ) ] += W_EXC * p2 [ ix ] [ iy ] * p2 [ jx ] [ jy ];
      }
    }
  }
}

void setInput ( float * i_ext )
{
  for ( int32_t i = 0; i < NX; i++ ) {
    for ( int32_t j = 0; j < NY; j++ ) {
      i_ext [ j + NY * i ] = ( j < NX / 2 ) ? I_EXT * p1 [ i ] [ j ] : 0;
    }
  }
}

void printSpikes ( const int32_t ns [ ] )
{
  for ( int32_t x = 0; x < NX; x++ ) {
    for ( int32_t y = 0; y < NY; y++ ) {
      printf ( "%d %d %d\n", x, y, ns [ y + NY * x ] );
    }
    printf ( "\n" );
  }
}

int main ( void )
{
  float *w = ( float * ) calloc ( N * N, sizeof ( float ) );
  createConnection ( w );

  float *i_ext = ( float * ) calloc ( N, sizeof ( float ) );
  setInput ( i_ext );

  float *v = ( float * ) calloc ( N, sizeof ( float ) );
  for ( int32_t i = 0; i < N; i++ ) { v [ i ] = E_REST; }

  float *g_syn = ( float * ) calloc ( N, sizeof ( float ) );
  bool *s = ( bool * ) calloc ( N, sizeof ( bool ) ); // spike 0 / 1
  int32_t *ns = ( int32_t * ) calloc ( N, sizeof ( int32_t ) ); // spike 0 / 1

  sfmt_t rng;
  sfmt_init_gen_rand ( &rng, getpid () );

  FILE *file [ N ];
  for ( int32_t i = 0; i < N; i++ ) {
    char buf [ 128 ];
    sprintf ( buf, "%d.dat", i );
    file [ i ] = fopen ( buf, "w" );
  }
  for ( int32_t nt = 0; nt < NT; nt++ ) {
    for ( int32_t i = 0; i < N; i++ ) {
      float r = 0;
      for ( int32_t j = 0; j < N; j++ ) {
	int32_t x = wid ( i, j );
	r += w [ x ] * s [ j ];
      }
      g_syn [ i ] = DECAY * g_syn [ i ] + r;
    }
    for ( int32_t i = 0; i < N; i++ ) {
      fprintf ( file [ i ], "%d %f\n", nt, v [ i ] );
      v [ i ] += DT * ( - ( v [ i ] - E_REST ) + g_syn [ i ] + R_M * ( i_ext [ i ] + sfmt_genrand_real2 ( &rng ) ) ) / TAU;
      s [ i ] = ( v [ i ] > THETA );
      if ( s [ i ] ) { fprintf ( file [ i ], "%d %f\n%d %f\n", nt + 1, v [ i ], nt + 1, 0. ); }
      v [ i ] = s [ i ] * ( E_REST ) + ( ! s [ i ] ) * ( v [ i ] );
      ns [ i ] += s [ i ];
    }
  }
  for ( int32_t i = 0; i < N; i++ ) {
    fclose ( file [ i ] );
  }

  printSpikes ( ns );

  free ( ns );
  free ( s );
  free ( g_syn );
  free ( v );
  free ( i_ext );
  free ( w );
}
