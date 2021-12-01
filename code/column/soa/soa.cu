#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "hh.h"

#define N     ( 65536 )
#define I_EXT ( 9.0 )
#define NT    ( 100000 ) // 100000 * 0.01 = 1000 ms
#define DT    ( 0.01 ) // ms

#define BLOCK_SIZE ( 32 )

typedef struct {
  double *v, *m, *h, *n;
} hh_t;

__device__ double dev_alpha_m ( const double v ) { return ( 2.5 - 0.1 * v ) / ( exp ( 2.5 - 0.1 * v ) - 1.0 ); }
__device__ double dev_beta_m  ( const double v ) { return 4.0 * exp ( - v / 18.0 ); }
__device__ double dev_alpha_h ( const double v ) { return 0.07 * exp ( - v / 20.0 ); }
__device__ double dev_beta_h  ( const double v ) { return 1.0 / ( exp ( 3.0 - 0.1 * v ) + 1.0 ); }
__device__ double dev_alpha_n ( const double v ) { return ( 0.1 - 0.01 * v ) / ( exp ( 1 - 0.1 * v ) - 1.0 ); }
__device__ double dev_beta_n  ( const double v ) { return 0.125 * exp ( - v / 80.0 ); }

extern "C" { void timer_start ( void ); }
extern "C" { double timer_elapsed ( void ); }

static void initialize ( hh_t *neuron )
{
  cudaMallocManaged ( &neuron -> v, N * sizeof ( double ) );
  cudaMallocManaged ( &neuron -> m, N * sizeof ( double ) );
  cudaMallocManaged ( &neuron -> h, N * sizeof ( double ) );
  cudaMallocManaged ( &neuron -> n, N * sizeof ( double ) );

  double v = E_LEAK;
  for ( int32_t i = 0; i < N; i++ ) {
    neuron -> v [ i ] = v;
    neuron -> m [ i ] = inf_m ( v );
    neuron -> h [ i ] = inf_h ( v );
    neuron -> n [ i ] = inf_n ( v );
  }
}

static void finalize ( hh_t *neuron )
{
  cudaFree ( neuron -> v );
  cudaFree ( neuron -> m );
  cudaFree ( neuron -> h );
  cudaFree ( neuron -> n );
}

__global__ void kernel_gpu ( hh_t *neuron )
{
  int32_t i = threadIdx.x + blockIdx.x * blockDim.x;

  if ( i < N ) {
    double *v = &neuron -> v [ i ];
    double *m = &neuron -> m [ i ];
    double *h = &neuron -> h [ i ];
    double *n = &neuron -> n [ i ];

    double dv = DT * ( 1.0 / C ) * ( - G_LEAK * ( *v - E_LEAK )
				     - G_NA * *m * *m * *m * *h * ( *v - E_NA )
				     - G_K * *n * *n * *n * *n * ( *v - E_K )
				     + I_EXT );
    double dm = DT * ( dev_alpha_m ( *v ) * ( 1.0 - *m ) - dev_beta_m ( *v ) * *m );
    double dh = DT * ( dev_alpha_h ( *v ) * ( 1.0 - *h ) - dev_beta_h ( *v ) * *h );
    double dn = DT * ( dev_alpha_n ( *v ) * ( 1.0 - *n ) - dev_beta_n ( *v ) * *n );

    neuron -> v [ i ] += dv;
    neuron -> m [ i ] += dm;
    neuron -> h [ i ] += dh;
    neuron -> n [ i ] += dn;
  }
}

static void kernel_cpu ( hh_t *neuron )
{

  for ( int32_t i = 0; i < N; i++ ) {
    double *v = &neuron -> v [ i ];
    double *m = &neuron -> m [ i ];
    double *h = &neuron -> h [ i ];
    double *n = &neuron -> n [ i ];

    double dv = DT * ( 1.0 / C ) * ( - G_LEAK * ( *v - E_LEAK )
				     - G_NA * *m * *m * *m * *h * ( *v - E_NA )
				     - G_K * *n * *n * *n * *n * ( *v - E_K )
				     + I_EXT );
    double dm = DT * ( alpha_m ( *v ) * ( 1.0 - *m ) - beta_m ( *v ) * *m );
    double dh = DT * ( alpha_h ( *v ) * ( 1.0 - *h ) - beta_h ( *v ) * *h );
    double dn = DT * ( alpha_n ( *v ) * ( 1.0 - *n ) - beta_n ( *v ) * *n );

    *v += dv;
    *m += dm;
    *h += dh;
    *n += dn;
  }
}

int main ( void )
{
  hh_t *neuron;
  cudaMallocManaged ( &neuron, sizeof ( hh_t ) );

  initialize ( neuron );

  int32_t gs = ( N + ( BLOCK_SIZE - 1) ) / BLOCK_SIZE;

  timer_start ();
  for ( int32_t nt = 0; nt < NT; nt++ ) {
    double t = DT * nt;
    //printf ( "%f %f\n", t, neuron -> v [ 0 ] );
    kernel_gpu <<< gs, BLOCK_SIZE >>> ( neuron );
    //kernel_cpu ( &neuron );
  }
  double elapsedTime = timer_elapsed ();
  printf ( "Elapsed time = %f sec.\n", elapsedTime);

  finalize ( neuron );
}
