#include "gr.h"

gr_t *initialize_gr ( void )
{
  gr_t *gr = ( gr_t * ) calloc ( 1, sizeof ( gr_t ) );
  *gr = ( gr_t ) { .v = ( float * ) calloc ( N_GR, sizeof ( float ) ),
		   .g_ampa = ( float * ) calloc ( N_GR, sizeof ( float ) ),
		   .g_nmda = ( float * ) calloc ( N_GR, sizeof ( float ) ),
		   .g_inh1 = ( float * ) calloc ( N_GR, sizeof ( float ) ),
		   .g_inh2 = ( float * ) calloc ( N_GR, sizeof ( float ) ),
		   .g_ahp  = ( float * ) calloc ( N_GR, sizeof ( float ) ),
		   .s      = ( bool  * ) calloc ( N_GR, sizeof ( bool  ) ),
  };
  for (int32_t i = 0; i < N_GR; i++ ) { gr -> v [ i ] = E_LEAK_GR; }
  gr -> n = N_GR;
  gr -> n_cluster = N_GR_CLUSTER;

  return gr;
}

void finalize_gr ( gr_t *gr )
{
  free ( gr -> v );
  free ( gr -> g_ampa );
  free ( gr -> g_nmda );
  free ( gr -> g_inh1 );
  free ( gr -> g_inh2 );
  free ( gr -> g_ahp );
}

void calc_syn_gr ( const float t, sfmt_t *rng, gr_t *gr, int32_t n_go, int32_t *c_grgo, bool *go )
{
  {
    const float c_ampa = DT / TAU_AMPA_GR;
    const float c_nmda = DT / TAU_NMDA_GR;
    const float decay_ampa = exp ( - c_ampa );
    const float decay_nmda = exp ( - c_nmda );
    for ( int32_t i = 0; i < N_GR; i++ ) {
      float r = 0.;
      for ( int32_t j = 0; j < N_MF_PER_GR; j++ ) {
	float firing_rate_tonic  = ( T_CS_START <= t && t < T_CS_END ) ? MF_FR_TONIC  : MF_FR_BASE;
	float firing_rate_phasic = ( T_CS_START <= t && t < T_CS_START + 1 ) ? MF_FR_PHASIC : MF_FR_BASE;
	bool mf = ( j < 2 ) ? ( sfmt_genrand_real2 ( rng ) < firing_rate_tonic ) : ( sfmt_genrand_real2 ( rng ) < firing_rate_phasic );
	r += CONN_W_GRMF * mf;
      }
      gr -> g_ampa [ i ] = decay_ampa * gr -> g_ampa [ i ] + c_ampa * r;
      gr -> g_nmda [ i ] = decay_nmda * gr -> g_nmda [ i ] + c_nmda * r;
    }
  }
  {
    const float c_inh1 = DT / TAU_INH1_GR;
    const float c_inh2 = DT / TAU_INH2_GR;
    const float decay_inh1 = exp ( - c_inh1 );
    const float decay_inh2 = exp ( - c_inh2 );
    for ( int32_t i = 0; i < n_go; i++ ) {
      float r = 0.;
      for ( int32_t j = 0; c_grgo [ j + ( n_go + 1 ) * i ] != -1; j++ ) {
	r += CONN_W_GRGO * go [ c_grgo [ j + ( n_go + 1 ) * i ] ];
      }
      for ( int32_t j = 0; j < N_GR_CLUSTER; j++ ) {
	int32_t k = j + N_GR_CLUSTER * i;
	gr -> g_inh1 [ k ] = decay_inh1 * gr -> g_inh1 [ k ] + c_inh1 * r;
	gr -> g_inh2 [ k ] = decay_inh2 * gr -> g_inh2 [ k ] + c_inh2 * r;
      }
    }
  }
  {
    const float c_ahp = DT / TAU_AHP_GR;
    const float decay_ahp  = exp ( - c_ahp );
    for ( int32_t i = 0; i < N_GR; i++ ) {
      gr -> g_ahp [ i ] = ( gr -> s [ i ] ) ? c_ahp : decay_ahp * gr -> g_ahp [ i ];
    }
  }
}

void update_gr ( gr_t *gr )
{
  const float r_ampa = 1. / ( DT / TAU_AMPA_GR );
  const float r_nmda = 1. / ( DT / TAU_NMDA_GR );
  const float r_inh1 = 1. / ( DT / TAU_INH1_GR );
  const float r_inh2 = 1. / ( DT / TAU_INH2_GR );
  const float r_ahp  = 1. / ( DT / TAU_AHP_GR );

  for ( int32_t i = 0; i < N_GR; i++ ) {
    float v = gr -> v [ i ];
    float k1 = DT * ( - G_LEAK_GR * ( v - E_LEAK_GR )
			    - G_EXC_GR * R_AMPA_GR * r_ampa * gr -> g_ampa [ i ] * ( v - E_EXC_GR )
			    - G_EXC_GR * R_NMDA_GR * r_nmda * gr -> g_nmda [ i ] * ( v - E_EXC_GR )
			    - G_INH_GR * R_INH1_GR * r_inh1 * gr -> g_inh1 [ i ] * ( v - E_INH_GR )
			    - G_INH_GR * R_INH2_GR * r_inh2 * gr -> g_inh2 [ i ] * ( v - E_INH_GR )
			    - G_AHP_GR * r_ahp * gr -> g_ahp [ i ] * ( v - E_AHP_GR )
			    ) / C_GR;
    float k2 = DT * ( - G_LEAK_GR * ( ( v + k1 ) - E_LEAK_GR )
			    - G_EXC_GR * R_AMPA_GR * r_ampa * gr -> g_ampa [ i ] * ( ( v + k1 ) - E_EXC_GR )
			    - G_EXC_GR * R_NMDA_GR * r_nmda * gr -> g_nmda [ i ] * ( ( v + k1 ) - E_EXC_GR )
			    - G_INH_GR * R_INH1_GR * r_inh1 * gr -> g_inh1 [ i ] * ( ( v + k1 ) - E_INH_GR )
			    - G_INH_GR * R_INH2_GR * r_inh2 * gr -> g_inh2 [ i ] * ( ( v + k1 ) - E_INH_GR )
			    - G_AHP_GR * r_ahp * gr -> g_ahp [ i ] * ( ( v + k1 ) - E_AHP_GR )
			    ) / C_GR;

    gr -> v [ i ] += .5 * ( k1 + k2 );
    gr -> s [ i ] = ( gr -> v [ i ] > THETA_GR );
  }
}
