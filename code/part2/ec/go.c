#include "go.h"

go_t *initialize_go ( void )
{
  go_t *go = ( go_t * ) calloc ( 1, sizeof ( go_t ) );
  *go = ( go_t ) { .v = ( float * ) calloc ( N_GO, sizeof ( float ) ),
		   .g_ampa  = ( float * ) calloc ( N_GO, sizeof ( float ) ),
		   .g_nmda1 = ( float * ) calloc ( N_GO, sizeof ( float ) ),
		   .g_nmda2 = ( float * ) calloc ( N_GO, sizeof ( float ) ),
		   .s       = ( bool  * ) calloc ( N_GO, sizeof ( bool  ) ),
		   .g_ahp   = ( float * ) calloc ( N_GO, sizeof ( float ) ) };
  for ( int32_t i = 0; i < N_GO; i++ ) { go -> v [ i ] = E_LEAK_GO; }
  go -> n = N_GO;

  return go;
}

void finalize_go ( go_t *go )
{
  free ( go -> v );
  free ( go -> g_ampa );
  free ( go -> g_nmda1 );
  free ( go -> g_nmda2 );
  free ( go -> s );
  free ( go -> g_ahp );
}

void calc_syn_go ( const float t, go_t *go, int32_t n_gr_per_cluster, int32_t *c_gogr, bool *gr )
{
  {
    const float c_ampa  = DT / TAU_AMPA_GO;
    const float c_nmda1 = DT / TAU_NMDA1_GO;
    const float c_nmda2 = DT / TAU_NMDA2_GO;
    const float decay_ampa  = exp ( - c_ampa  );
    const float decay_nmda1 = exp ( - c_nmda1 );
    const float decay_nmda2 = exp ( - c_nmda2 );
    for ( int32_t i = 0; i < N_GO; i++ ) {
      float r = 0.;
      for ( int32_t j = 0; c_gogr [ j + ( N_GO + 1 ) * i ] != -1; j++ ) { // N_GO is correct
	int32_t l = c_gogr [ j + ( N_GO + 1 ) * i ];
	for ( int32_t k = 0; k < n_gr_per_cluster; k++ ) {
	  r += CONN_W_GOGR * gr [ k + n_gr_per_cluster * l ];
	}
      }
      go -> g_ampa  [ i ] = decay_ampa  * go -> g_ampa  [ i ] + c_ampa  * r;
      go -> g_nmda1 [ i ] = decay_nmda1 * go -> g_nmda1 [ i ] + c_nmda1 * r;
      go -> g_nmda2 [ i ] = decay_nmda2 * go -> g_nmda2 [ i ] + c_nmda2 * r;
    }
  }

  {
    const float c_ahp = DT / TAU_AHP_GO;
    const float decay_ahp = exp ( - c_ahp );
    for ( int32_t i = 0; i < N_GO; i++ ) {
      go -> g_ahp [ i ] =  ( go -> s [ i ] ) ? c_ahp : decay_ahp * go -> g_ahp [ i ];
    }
  }
}

void update_go ( go_t *go )
{
  const float r_ampa  = 1. / ( DT / TAU_AMPA_GO );
  const float r_nmda1 = 1. / ( DT / TAU_NMDA1_GO );
  const float r_nmda2 = 1. / ( DT / TAU_NMDA2_GO );
  const float r_ahp   = 1. / ( DT / TAU_AHP_GO );

  for ( int32_t i = 0; i < N_GO; i++ ) {
    float v = go -> v [ i ];
    float k1 = DT * ( - G_LEAK_GO * ( v - E_LEAK_GO )
		      - G_EXC_GO * R_AMPA_GO  * r_ampa  * go -> g_ampa  [ i ] * ( v - E_EXC_GO )
		      - G_EXC_GO * R_NMDA1_GO * r_nmda1 * go -> g_nmda1 [ i ] * ( v - E_EXC_GO )
		      - G_EXC_GO * R_NMDA2_GO * r_nmda2 * go -> g_nmda2 [ i ] * ( v - E_EXC_GO )
		      - G_AHP_GO * R_AHP_GO   * r_ahp   * go -> g_ahp   [ i ] * ( v - E_AHP_GO )
		      ) / C_GO;
    float k2 = DT * ( - G_LEAK_GO * ( ( v + k1 ) - E_LEAK_GO )
		      - G_EXC_GO * R_AMPA_GO  * r_ampa  * go -> g_ampa  [ i ] * ( ( v + k1 ) - E_EXC_GO )
		      - G_EXC_GO * R_NMDA1_GO * r_nmda1 * go -> g_nmda1 [ i ] * ( ( v + k1 ) - E_EXC_GO )
		      - G_EXC_GO * R_NMDA2_GO * r_nmda2 * go -> g_nmda2 [ i ] * ( ( v + k1 ) - E_EXC_GO )
		      - G_AHP_GO * R_AHP_GO   * r_ahp   * go -> g_ahp   [ i ] * ( ( v + k1 ) - E_AHP_GO )
		      ) / C_GO;

    go -> v [ i ] += .5 * ( k1 + k2 ); 
    go -> s [ i ] = ( go -> v [ i ] > THETA_GO );
  }
}
