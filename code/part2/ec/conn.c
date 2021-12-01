#include "conn.h"

int32_t *initialize_grgo ( sfmt_t *rng )
{

  bool *w = ( bool * ) calloc ( N_GO * N_GO, sizeof ( bool ) );

  for ( int32_t dst_x = 0; dst_x < X_GO; dst_x++ ) {
    for ( int32_t dst_y = 0; dst_y < Y_GO; dst_y++ ) {
      int32_t dst = dst_y + Y_GO * dst_x;
      for ( int32_t dx = -4; dx <= 4; dx++ ) {
	for ( int32_t dy = -4; dy <= 4; dy++ ) {
	  int32_t src_x = dst_x + dx;
	  src_x += ( src_x >= X_GO ) * ( - X_GO ) + ( src_x < 0 ) * ( X_GO );
	  int32_t src_y = dst_y + dy;
	  src_y += ( src_y >= Y_GO ) * ( - Y_GO ) + ( src_y < 0 ) * ( Y_GO );
	  int32_t src = src_y + Y_GO * src_x;
	  w [ dst + N_GO * src ] = ( sfmt_genrand_real2 ( rng ) < CONN_PROB_GRGO );
	}
      }
    }
  }

  int32_t *list_grgo = ( int32_t * ) calloc ( N_GO * ( N_GO + 1 ), sizeof ( int32_t ) );
  for ( int32_t i = 0; i < N_GO; i++ ) { list_grgo [ 0 + ( N_GO + 1 ) * i ] = -1; }

  for ( int32_t dst_x = 0; dst_x < X_GO; dst_x++ ) {
    for ( int32_t dst_y = 0; dst_y < Y_GO; dst_y++ ) {
      int32_t dst = dst_y + Y_GO * dst_x;
      int k = 0;
      for ( int32_t dx = 0; dx <= 1; dx++ ) {
	for ( int32_t dy = 0; dy <= 1; dy++ ) {
	  int32_t src_x = dst_x + dx;
	  src_x += ( src_x >= X_GO ) * ( - X_GO );
	  int32_t src_y = dst_y + dy;
	  src_y += ( src_y >= Y_GO ) * ( - Y_GO );
	  int32_t src = src_y + Y_GO * src_x;
	  for ( int i = 0; i < N_GO; i++ ) {
	    if ( w [ i + N_GO * src ] ) {
	      list_grgo [ k + ( N_GO + 1 ) * dst ] = i;
	      k++;
	      list_grgo [ k + ( N_GO + 1 ) * dst ] = -1; // Sentinel
	    }
	  }
	}
      }
    }
  }

  free ( w );
  return list_grgo;
}

int32_t *initialize_gogr ( sfmt_t *rng )
{
  int32_t *list_gogr = ( int32_t * ) calloc ( N_GO * ( N_GO + 1 ), sizeof ( int32_t ) );
  for ( int32_t i = 0; i < N_GO; i++ ) { list_gogr [ 0 + ( N_GO + 1 ) * i ] = -1; }

  for ( int32_t dst_x = 0; dst_x < X_GO; dst_x++ ) {
    for ( int32_t dst_y = 0; dst_y < Y_GO; dst_y++ ) {
      int32_t dst = dst_y + Y_GO * dst_x;
      int32_t k = 0;
      for ( int32_t dx = -3; dx <= 3; dx++ ) {
	int32_t src_x = dst_x + dx;
	src_x += ( src_x >= X_GO ) * ( - X_GO ) + ( src_x < 0 ) * ( X_GO );
	for ( int32_t dy = -3; dy <= 3; dy++ ) {
	  int32_t src_y = dst_y + dy;
	  src_y += ( src_y >= Y_GO ) * ( - Y_GO ) + ( src_y < 0 ) * ( Y_GO );
	  int32_t src = src_y + Y_GO * src_x;
	  if ( sfmt_genrand_real2 ( rng ) < CONN_PROB_GOGR ) {
	    list_gogr [ k + ( N_GO + 1 ) * dst ] = src;
	    k++;
	    list_gogr [ k + ( N_GO + 1 ) * dst ] = -1; // Sentinel
	  }
	}
      }
    }
  }

  return list_gogr;
}

float *initialize_pcgr ( sfmt_t *rng )
{
  float *w = ( float * ) calloc ( N_PC * N_GR, sizeof ( float ) );

  for ( int32_t pc_y = 0; pc_y < Y_PC; pc_y++ ) {
    for ( int32_t pc_x = 0; pc_x < X_PC; pc_x++ ) {
      int32_t pc = pc_y + Y_PC * pc_x;
      int32_t ax = ( ( X_GO ) / ( X_PC ) ) * pc_x;
      for ( int32_t dx = -4; dx <= 4; dx++ ) {
        int32_t go_x = ax + dx;
	go_x += ( go_x >= X_GO ) * ( - X_GO ) + ( go_x < 0 ) * ( X_GO );
        for ( int32_t go_y = 0; go_y < Y_GO; go_y++ ) {
          int32_t go = go_y + Y_GO * go_x;
          for (int32_t n = 0; n < N_GR_CLUSTER; n++ ) {
            int32_t gr = n + N_GR_CLUSTER * go;
	    w [ gr + N_GR * pc] = 1.;
          }
        }
      }
    }
  }

  return w;
}

void finalize_grgo ( int32_t *list_grgo ) { free ( list_grgo ); }
void finalize_gogr ( int32_t *list_gogr ) { free ( list_gogr ); }
void finalize_pcgr ( float   *w_gogr    ) { free ( w_gogr ); }
