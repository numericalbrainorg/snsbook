#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#include "param.h"
#include "gr.h"
#include "go.h"
#include "pc.h"
#include "conn.h"

#define C_LTP ( 0.001 )
#define C_LTD ( 0.8 )
#define LTD_WINDOW_SIZE ( 50 )

void LTP ( float *w_pcgr, bool *s )
{
  for ( int32_t j = 0; j < N_GR; j++ ) {
    if ( s [ j ] ) {
      for ( int32_t i = 0; i < N_PC; i++ ) {
	if ( w_pcgr [ j + N_GR * i ] > 0 ) {
	  w_pcgr [ j + N_GR * i ] += C_LTP * ( 1. - w_pcgr [ j + N_GR * i ] );
	}
      }
    }
  }
}

void LTD ( float *w_pcgr, int32_t *grs_for_ltd )
{
  for ( int32_t j = 0; grs_for_ltd [ j ] != -1; j++ ) {
    for ( int32_t i = 0; i < N_PC; i++ ) {
      if ( w_pcgr [ grs_for_ltd [ j ] + N_GR * i ] > 0 ) {
	w_pcgr [ grs_for_ltd [ j ] + N_GR * i ] *= C_LTD;
      }
    }
  }
}

int main ( void )
{
  sfmt_t rng;
  sfmt_init_gen_rand ( &rng, 23L );

  gr_t *gr = initialize_gr ( );
  go_t *go = initialize_go ( );
  pc_t *pc = initialize_pc ( );

  int32_t *c_gogr = initialize_gogr ( &rng );
  int32_t *c_grgo = initialize_grgo ( &rng );
  float   *w_pcgr = initialize_pcgr ( &rng );

  int32_t *grs_for_ltd   = ( int32_t * ) calloc ( LTD_WINDOW_SIZE * gr -> n, sizeof ( int32_t ) );

  for ( int32_t ntrial = 0; ntrial < N_TRIAL; ntrial++ ) {
    fprintf ( stderr, "trial = %d\n", ntrial );
    char filename [ 256 ];
    sprintf ( filename, "gr.spk.%d", ntrial );
    FILE *fgr = fopen ( filename, "w" );
    sprintf ( filename, "go.spk.%d", ntrial );
    FILE *fgo = fopen ( filename, "w" );
    sprintf ( filename, "pc.spk.%d", ntrial );
    FILE *fpc = fopen ( filename, "w" );
    sprintf ( filename, "pc.mbp.%d", ntrial );
    FILE *fmbp = fopen ( filename, "w" );

    int32_t grs_idx = 0;
    grs_for_ltd [ grs_idx ] = -1;

    for ( int32_t nt = 0; nt < NT; nt++ ) {
      float t = DT * nt;

      calc_syn_gr ( t, &rng, gr, go -> n,         c_grgo, go -> s );
      calc_syn_go ( t,       go, gr -> n_cluster, c_gogr, gr -> s );
      calc_syn_pc ( t,       pc, gr -> n,         w_pcgr, gr -> s );
      update_gr ( gr );
      update_go ( go );
      update_pc ( pc );

      if ( ( T_US - LTD_WINDOW_SIZE <= t ) && ( t < T_US ) ) {
	for ( int32_t i = 0; i < gr -> n; i++ ) {
	  if ( gr -> s [ i ] ) {
	    grs_for_ltd [ grs_idx ] = i;
	    grs_idx++;
	    grs_for_ltd [ grs_idx ] = -1;
	  }
	}
      }

      for ( int32_t i = 0; i < gr -> n; i += gr -> n_cluster ) {
	if ( gr -> s [ i ] ) { fprintf ( fgr, "%f %d\n", DT * nt, i / gr -> n_cluster ); }
      }
/*
      for ( int32_t i = 0; i < gr -> n; i++ ) {
	if ( gr -> s [ i ] ) { fprintf ( fgr, "%f %d\n", DT * nt, i ); }
      }
*/
      for ( int32_t i = 0; i < go -> n; i++ ) {
	if ( go -> s [ i ] ) { fprintf ( fgo, "%f %d\n", DT * nt, i ); }
      }

      for ( int32_t i = 0; i < pc -> n; i++ ) {
	if ( pc -> s [ i ] ) { fprintf ( fpc, "%f %d\n", DT * nt, i ); }
      }

      {
	fprintf ( fmbp, "%f ", DT * nt );
	for ( int32_t i = 0; i < pc -> n; i++ ) {
	  fprintf ( fmbp, "%f%s", pc -> v [ i ], ( i == pc -> n - 1 ) ? "\n" : " " );
	}
      }

      LTP ( w_pcgr, gr -> s );

      fflush ( fgr );
      fflush ( fgo );
      fflush ( fpc );
      fflush ( fmbp );
    }
    
    LTD ( w_pcgr, grs_for_ltd );

    fclose ( fgr );
    fclose ( fgo );
    fclose ( fpc );
    fclose ( fmbp );
  }

  finalize_grgo ( c_grgo );
  finalize_gogr ( c_gogr );
  finalize_pcgr ( w_pcgr );
  finalize_gr ( gr );
  finalize_go ( go );
  finalize_pc ( pc );
}
