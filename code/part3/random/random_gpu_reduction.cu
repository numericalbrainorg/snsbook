#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>

#define N       ( 4000 )            // # of neurons 
#define N_E     ( 3200 )            // # of excitatory neurons
#define N_I     ( ( N ) - ( N_E ) ) // # of inhibitory neurons 

#define T       ( 1000 ) // total simulation time ( 1000 ms, unused )
#define DT      ( 1.   ) // delta t = 1 ms
#define NT      ( 1000 ) // # of steps ( = T / DT )

#define TAU_M   (  20. ) // membrane time constant ( ms )
#define TAU_E   (  5.  ) // excitatory synapse time constant ( ms )
#define TAU_I   (  10. ) // inhibitory synapse time constant ( ms )

#define V_REST  ( -49. ) // resting potential ( mV )
#define V_INIT  ( -60. ) // initial value ( mV )
#define V_RESET ( -60. ) // reset potential ( mV )
#define THETA   ( -50. ) // threshold ( mV )

#define G_E     ( ( 0.1 * 1.62 ) / ( TAU_E ) ) // 1 / peak excitatory synaptic conductance
#define G_I     ( ( 0.1 * -9.  ) / ( TAU_I ) ) // 1 / peak inhibitory synaptic conductance
#define P       ( 0.2 ) // probability of making synaptic connections

typedef struct {
  float *v, *ge, *gi, *w;
  bool *s;
  int32_t *wc;
  int32_t nc;
  sfmt_t rng;
  FILE *file;
} network_t;

extern "C" { void timer_start ( void ); }
extern "C" { double timer_elapsed ( void ); }

#define BLOCK_SIZE ( 32 )
#define GRID_SIZE  ( ( ( N ) + ( BLOCK_SIZE ) - 1 ) / ( BLOCK_SIZE ) )
#define NTHSYN ( 2048 )

void initialize ( network_t *n )
{
  // PRNG
  sfmt_init_gen_rand ( &n -> rng, 23 );

  // File
  n -> file = fopen ( "spike.dat", "w");

  // Cell parameters
  cudaMallocManaged ( &n -> v,  N * sizeof ( float ) );
  cudaMallocManaged ( &n -> ge, N * sizeof ( float ) );
  cudaMallocManaged ( &n -> gi, N * sizeof ( float ) );
  cudaMallocManaged ( &n -> s,  N * sizeof ( bool ) );

  for ( int i = 0; i < N; i++ ) {
    n -> v [ i ] = V_INIT + 10. * sfmt_genrand_real2 ( &n -> rng );
    n -> ge [ i ] = 0.;
    n -> gi [ i ] = 0.;
    n -> s [ i ] = 0;
  }

  // Synaptic connections

  // Temporary building connections
  float *w  = ( float * ) calloc ( N * N, sizeof ( float ) );
  int32_t *wc = ( int32_t * ) calloc ( N * N, sizeof ( int32_t ) );
  int32_t maxcol = 0;
  for ( int32_t i = 0; i < N; i++ ) {
    int32_t col = 0;
    for ( int32_t j = 0; j < N; j++ ) {
      if ( sfmt_genrand_real2 ( &n -> rng ) < P ) {
	w [ j + N * i ] = 1.;
	wc [ col + N * i ] = j;
	col++;
      }
    }
    wc [ col + N * i ] = -1; // sentinel
    if ( maxcol < col ) { maxcol = col; }
  }

  // Building ELL matrix
  n -> nc = maxcol + 1; // +1 for sentinel
  cudaMallocManaged ( &n -> w,  n -> nc * N * sizeof ( float ) );
  cudaMallocManaged ( &n -> wc, n -> nc * N * sizeof ( int32_t ) );
  for ( int32_t i = 0; i < N; i++ ) {
    int32_t j = 0, k = 0;
    while ( ( k = wc [ j + N * i ] ) != -1 ) {
      n -> w  [ j + n -> nc * i ] = w [ k + N * i ];
      n -> wc [ j + n -> nc * i ] = k;
      j++;
    }
    while ( j < n -> nc ) {
      n -> w  [ j + n -> nc * i ] = 0; // filling 0
      n -> wc [ j + n -> nc * i ] = -1; // filling -1
      j++;
    }
  }

  free ( w );
  free ( wc );
}

void finalize ( network_t *n )
{
  cudaFree ( n -> v );
  cudaFree ( n -> ge );
  cudaFree ( n -> gi );
  cudaFree ( n -> s );
  cudaFree ( n -> w );
  cudaFree ( n -> wc );
  fclose ( n -> file );
}

__global__ void calculateSynapticInputs_a ( network_t *n )
{
  int32_t i = threadIdx.x + blockIdx.x * blockDim.x;

  if ( i < N ) {
    float re = 0, ri = 0;
    for ( int32_t j = 0, k = 0; ( k = n -> wc [ j + n -> nc * i ] ) != -1; j++ ){
      float r = n -> w [ j + n -> nc * i ] * n -> s [ k ];
      if ( k < N_E ) { re += r; } else { ri += r; } 
    }
    n -> ge [ i ] = exp ( - DT / TAU_E ) * n -> ge [ i ] + re;
    n -> gi [ i ] = exp ( - DT / TAU_I ) * n -> gi [ i ] + ri;
  }
}

__global__ void calculateSynapticInputs_b ( network_t *n )
{
  int32_t _i = threadIdx.x + blockIdx.x * blockDim.x;

  int32_t i = _i / NTHSYN;
  int32_t j = _i % NTHSYN;

  __shared__ float s_re [ NTHSYN ], s_ri [ NTHSYN ];

  if ( i < N ) {
    int32_t l = ( j < n -> nc ) ? n -> wc [ j + n -> nc * i] : -1;
    s_re [ j ] = ( l != -1 && l < N_E  ) ? n -> w [ j + n -> nc * i ] * n -> s [ l ] : 0.;
    s_ri [ j ] = ( l != -1 && l >= N_E ) ? n -> w [ j + n -> nc * i ] * n -> s [ l ] : 0.;
    for ( int32_t k = NTHSYN; k < n -> nc; k += NTHSYN ) {
      int32_t l = ( j + k < n -> nc ) ? n -> wc [ j + k + n -> nc * i] : -1;
      s_re [ j ] += ( l != -1 && l < N_E )  ? n -> w [ j + k + n -> nc * i ] * n -> s [ l ] : 0.;
      s_ri [ j ] += ( l != -1 && l >= N_E ) ? n -> w [ j + k + n -> nc * i ] * n -> s [ l ] : 0.;
    }
  }
  __syncthreads ( );

  for ( int32_t k = NTHSYN / 2; k > 0; k >>= 1 ) {
    if ( i < N && j < k ) {
      s_re [ j ] += s_re [ j + k ];
      s_ri [ j ] += s_ri [ j + k ];
    }
    __syncthreads ( );
  }

  if ( i < N && j == 0 ) {
    n -> ge [ i ] = exp ( - DT / TAU_E ) * n -> ge [ i ] + s_re [ 0 ];
    n -> gi [ i ] = exp ( - DT / TAU_I ) * n -> gi [ i ] + s_ri [ 0 ];
  }
}

__global__ void updateCellParameters ( network_t *n )
{
  int32_t i = threadIdx.x + blockIdx.x * blockDim.x;

  if ( i < N ) {
    n -> v [ i ] += DT * ( - ( n -> v [ i ] - V_REST ) + G_E * n -> ge [ i ] + G_I * n -> gi [ i ] ) / TAU_M;
    n -> s [ i ] = ( n -> v [ i ] > THETA );
    n -> v [ i ] = ( n -> s [ i ] ) * V_RESET + ( ! n -> s [ i ] ) * n -> v [ i ];
  }
}

void outputSpike ( const int32_t nt, network_t *n )
{
  for ( int32_t i = 0; i < N; i++ ) {
    if ( n -> s [ i ] ) { fprintf ( n -> file, "%f %d\n", DT * ( nt + 1 ), i ); } // Spike time is not t but t + DT
  }
}

void loop ( network_t *n )
{
  timer_start ( );

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    calculateSynapticInputs_a <<< GRID_SIZE, BLOCK_SIZE >>> ( n );
    //calculateSynapticInputs_b <<< N, NTHSYN >>> ( n );
    updateCellParameters <<< GRID_SIZE, BLOCK_SIZE >>> ( n );
    cudaDeviceSynchronize ( );
    outputSpike ( nt, n );
  }

  double elapsedTime = timer_elapsed ( );
  printf ( "Elapsed time = %f sec.\n", elapsedTime);
}

int main ( void )
{
  network_t *n;
  cudaMallocManaged ( &n, sizeof ( network_t ) );

  initialize ( n );
  loop ( n );
  finalize ( n );

  cudaFree ( n );
}
