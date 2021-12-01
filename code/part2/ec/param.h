#pragma once

#define N_TRIAL ( 50 )
#define T       ( 1000. )
#define DT      ( 1. )
#define NT      ( 1000 ) // = T / DT

#define N_GR_CLUSTER ( 100 )
#define N_GR ( 32 * 32 * ( N_GR_CLUSTER ) )
#define N_MF_PER_GR ( 4 )

#define X_GO ( 32 )
#define Y_GO ( 32 ) 
#define N_GO ( ( X_GO ) * ( Y_GO ) )

#define X_PC ( 16 )
#define Y_PC (  1 )
#define N_PC ( ( X_PC ) * ( Y_PC ) )

#define T_CS_START ( 250. )
#define T_CS_END   ( 750. )
#define T_US       ( 500. )
