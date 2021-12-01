#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#include "param.h"

#define THETA_GR (-35.0)
#define C_GR (3.1)
#define R_GR (2.3)
#define E_LEAK_GR (-58.0)
#define G_LEAK_GR (0.43)
#define E_EXC_GR (0)
#define G_EXC_GR (0.18)
#define E_INH_GR (-82.0)
#define G_INH_GR (0.028)
#define E_AHP_GR (-82.0)
#define G_AHP_GR (1.0)

#define R_AMPA_GR ( 0.88 )
#define R_NMDA_GR ( 0.12 )
#define R_INH1_GR ( 0.43 )
#define R_INH2_GR ( 0.57 )

#define TAU_AMPA_GR  (  1.2 )
#define TAU_NMDA_GR  ( 52.0 )
#define TAU_INH1_GR  (  7.0 )
#define TAU_INH2_GR  ( 59.0 )
#define TAU_AHP_GR   (  5.0 )

#define MF_FR_TONIC  ( 0.04 )
#define MF_FR_PHASIC ( 0.2 )
#define MF_FR_BASE   ( 0.005 )

#define CONN_W_GRMF (  3. ) //3.5 )
#define CONN_W_GRGO ( 20. ) //240.0 )

typedef struct {
  float *v, *g_ampa, *g_nmda, *g_inh1, *g_inh2, *g_ahp;
  bool *s;
  int32_t n, n_cluster;
} gr_t;

extern gr_t *initialize_gr ( void );
extern void calc_syn_gr ( const float, sfmt_t *, gr_t *, int32_t, int32_t *, bool * );
extern void update_gr ( gr_t * );
extern void finalize_gr ( gr_t * );
