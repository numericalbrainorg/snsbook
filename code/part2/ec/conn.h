#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#include "param.h"

#define CONN_PROB_GRGO ( 0.025 )
#define CONN_PROB_GOGR ( 0.05 )

extern int32_t *initialize_grgo ( sfmt_t * );
extern int32_t *initialize_gogr ( sfmt_t * );
extern float   *initialize_pcgr ( sfmt_t * );
extern void finalize_grgo ( int32_t * );
extern void finalize_gogr ( int32_t * );
extern void finalize_pcgr ( float   * );
