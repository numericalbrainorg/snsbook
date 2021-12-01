#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "param.h"

#define THETA_GO  ( -52.0 )
#define C_GO      (  28.0 )
#define E_LEAK_GO ( -55.0 )
#define G_LEAK_GO ( ( 1. / 0.428 ) )
#define E_EXC_GO  ( 0. )
#define G_EXC_GO  (  45.5 )
#define E_AHP_GO  ( -72.7 )
#define G_AHP_GO  (  20.0 )

#define R_AMPA_GO  ( 0.8 )
#define R_NMDA1_GO ( 0.2 * 0.33 )
#define R_NMDA2_GO ( 0.2 * 0.67 )
#define R_AHP_GO   ( 1.0 )
#define TAU_AMPA_GO   (   1.5 )
#define TAU_NMDA1_GO  (  31.0 )
#define TAU_NMDA2_GO  ( 170.0 )
#define TAU_AHP_GO    (   5.0 )

#define CONN_W_GOGR ( 0.005 / ( N_GR_CLUSTER ) )

typedef struct {
  float *v, *g_ampa, *g_nmda1, *g_nmda2, *g_ahp;
  bool *s;
  int32_t n;
} go_t;

extern go_t *initialize_go ( void );
extern void calc_syn_go ( const float, go_t *, int32_t, int32_t *, bool * );
extern void update_go ( go_t * );
extern void finalize_go ( go_t * );
