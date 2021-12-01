#include "pc.h"

pc_t *initialize_pc ( void )
{
  pc_t *pc = ( pc_t * ) calloc ( 1, sizeof ( pc_t ) );
  *pc = ( pc_t ) { .v = ( float * ) calloc ( N_PC, sizeof ( float ) ),
		   .g_ampa = ( float * ) calloc ( N_PC, sizeof ( float ) ),
		   .g_ahp  = ( float * ) calloc ( N_PC, sizeof ( float ) ),
		   .s      = ( bool  * ) calloc ( N_PC, sizeof ( bool  ) ), };
  for (int32_t i = 0; i < N_PC; i++ ) { pc -> v [ i ] = E_LEAK_PC; }
  pc -> n = N_PC;

  return pc;
}

void finalize_pc ( pc_t *pc )
{
  free ( pc -> v );
  free ( pc -> g_ampa );
  free ( pc -> g_ahp );
  free ( pc -> s );
}

void calc_syn_pc ( const float t, pc_t *pc, int32_t n_gr, float *w_pcgr, bool *gr )
{
  {
    const float c_ampa = DT / TAU_AMPA_PC;
    const float decay_ampa = exp ( - c_ampa );
    for ( int32_t i = 0; i < N_PC; i++ ) {
      float r = 0.;
      for ( int32_t j = 0; j < n_gr; j++ ) {
	r += CONN_W_PCGR * w_pcgr [ j + n_gr * i ] * gr [ j ];
      }
      pc -> g_ampa [ i ] = decay_ampa * pc -> g_ampa [ i ] + c_ampa * r;
    }
  }

  {
    const float c_ahp = DT / TAU_AHP_PC;
    const float decay_ahp = exp ( - c_ahp );
    for ( int32_t i = 0; i < N_PC; i++ ) {
      pc -> g_ahp [ i ] = ( pc -> s [ i ] ) ? c_ahp : decay_ahp * pc -> g_ahp [ i ];
    }
  }
}

void update_pc ( pc_t *pc )
{
  const float r_ampa = 1. / ( DT / TAU_AMPA_PC );
  const float r_ahp  = 1. / ( DT / TAU_AHP_PC );

  for ( int32_t i = 0; i < N_PC; i++ ) {
    float v = pc -> v [ i ];
    float k1 = DT * ( - G_LEAK_PC * ( v - E_LEAK_PC )
		      - G_EXC_PC  * r_ampa * pc -> g_ampa [ i ] * ( v - E_EXC_PC )
		      - G_AHP_PC  * r_ahp  * pc -> g_ahp  [ i ] * ( v - E_AHP_PC )
		      ) / C_PC;
    float k2 = DT * ( - G_LEAK_PC * ( ( v + k1 ) - E_LEAK_PC )
		      - G_EXC_PC  * r_ampa * pc -> g_ampa [ i ] * ( ( v + k1 ) - E_EXC_PC )
		      - G_AHP_PC  * r_ahp  * pc -> g_ahp  [ i ] * ( ( v + k1 ) - E_AHP_PC )
		      ) / C_PC;

    pc -> v [ i ] += .5 * ( k1 + k2 );
    pc -> s [ i ] = ( pc -> v [ i ] > THETA_PC );
  }
}
