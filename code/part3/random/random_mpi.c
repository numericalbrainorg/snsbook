#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>
#include <mpi.h>

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

#define MIN(a,b) ( ( ( a ) < ( b ) ) ? ( a ) : ( b ) )

typedef struct {
  float *v, *ge, *gi, *w;
  bool *s;
  int32_t *wc, nc;
  sfmt_t rng;
  FILE *file;
} network_t;

extern void timer_start ( void );
extern double timer_elapsed ( void );

void initialize ( const int mpi_size, const int mpi_rank, network_t *n )
{
  *n = ( network_t ) { . v  = calloc ( N, sizeof ( float ) ),
		       . ge = calloc ( N, sizeof ( float ) ),
		       . gi = calloc ( N, sizeof ( float ) ),
  		       //. s  = calloc ( N, sizeof ( bool ), ), // defined elsewhere
		       //. file = fopen ( "spike.dat", "w"), // defined elsewhere
  };

  // PRNG
  sfmt_init_gen_rand ( &n -> rng, 23 );

  // File
  if ( mpi_rank == 0 ) { n -> file = fopen ( "spike.dat", "w"); }

  // Membrane potentials
  for ( int32_t i = 0; i < N; i++ ) {
    n -> v [ i ] = V_INIT + 10. * sfmt_genrand_real2 ( &n -> rng );
  }

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
  n -> w  = ( float * ) calloc ( n -> nc * N, sizeof ( float ) );
  n -> wc = ( int32_t * ) calloc ( n -> nc * N, sizeof ( int32_t ) );
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

void finalize ( const int mpi_size, const int mpi_rank, network_t *n )
{
  free ( n -> v );
  free ( n -> ge );
  free ( n -> gi );
  free ( n -> s );
  free ( n -> w );
  free ( n -> wc );
  if ( mpi_rank == 0 ) { fclose ( n -> file ); }
}

void calculateSynapticInputs ( const int32_t n_each, const int32_t n_offset, network_t *n )
{
  for ( int32_t i = n_offset; i < MIN ( n_offset + n_each, N ); i++ ) {
    float re = 0, ri = 0;
    for ( int32_t j = 0, k = 0; ( k = n -> wc [ j + n -> nc * i ] ) != -1; j++ ){
      float r = n -> w [ j + n -> nc * i ] * n -> s [ k ];
      if ( k < N_E ) { re += r; } else { ri += r; } 
    }
    n -> ge [ i ] = exp ( - DT / TAU_E ) * n -> ge [ i ] + re;
    n -> gi [ i ] = exp ( - DT / TAU_I ) * n -> gi [ i ] + ri;
  }
}

void updateCellParameters ( const int32_t n_each, const int32_t n_offset, network_t *n )
{
  for ( int32_t i = n_offset; i < MIN ( n_offset + n_each, N ); i++ ) {
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

void loop ( const int mpi_size, const int mpi_rank, network_t *n )
{
  const int32_t n_each = ( N + mpi_size - 1 ) / mpi_size;
  const int32_t n_offset = n_each * mpi_rank;
  n -> s  = calloc ( n_each * mpi_size, sizeof ( bool ) );

  bool s_local [ n_each ];
 
  timer_start ( );

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    calculateSynapticInputs ( n_each, n_offset, n );
    updateCellParameters ( n_each, n_offset, n );
    for ( int32_t i = n_offset; i < MIN ( n_offset + n_each, N ); i++ ) { s_local [ i - n_offset ] = n -> s [ i ]; }
    MPI_Allgather ( s_local, n_each, MPI_C_BOOL, n -> s, n_each, MPI_C_BOOL, MPI_COMM_WORLD );
    if ( mpi_rank == 0 ) { outputSpike ( nt, n ); }
  }

  double elapsedTime = timer_elapsed ( );
  if ( mpi_rank == 0 ) { printf ( "Elapsed time = %f sec.\n", elapsedTime); }
}

int main ( int argc, char *argv [ ] )
{
  MPI_Init ( &argc, &argv );
  int mpi_size, mpi_rank;
  MPI_Comm_size ( MPI_COMM_WORLD, &mpi_size );
  MPI_Comm_rank ( MPI_COMM_WORLD, &mpi_rank );

  network_t n;

  initialize ( mpi_size, mpi_rank, &n );
  loop ( mpi_size, mpi_rank, &n );
  finalize ( mpi_size, mpi_rank, &n );

  MPI_Finalize ( );
}
