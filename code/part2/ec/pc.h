#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>

#include "param.h"

#define THETA_PC    ( -55. )
#define C_PC        ( 106. )
#define G_LEAK_PC   ( 1. / 0.431 )
#define E_LEAK_PC   ( -68. )
#define G_EXC_PC    ( 0.7  )
#define E_EXC_PC    ( 0.   )
#define G_AHP_PC    ( 100. )
#define E_AHP_PC    ( -70. )

#define TAU_AHP_PC  ( 2.5 )
#define TAU_AMPA_PC ( 8.3 )

#define CONN_W_PCGR ( 0.005 )

typedef struct {
  float *v, *g_ampa, *g_ahp;
  bool *s;
  int32_t n;
} pc_t;

extern pc_t *initialize_pc ( void );
extern void calc_syn_pc ( const float, pc_t *, int32_t, float *, bool * );
extern void update_pc ( pc_t * );
extern void finalize_pc ( pc_t * );
