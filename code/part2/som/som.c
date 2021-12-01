#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdint.h>
#include <stdbool.h>
#include <SFMT.h>
#include "mnist.h"

#define DT 1.
#define NT 100

#define N_PRE ( MNIST_IMAGE_SIZE )
#define N_POST_X ( 32 )
#define N_POST_Y ( 32 )
#define N_POST ( ( N_POST_X ) * ( N_POST_Y ) )

#define TAU_M   (  20. )
#define V_LEAK  ( -49. )
#define V_INIT  ( -60. )
#define V_RESET ( -60. )
#define THETA   ( -50. )
#define G_SYN   (   5. )

#define ETA     ( 1.0  )
#define SIGMA2  ( 1.0  )

typedef struct {
  float *w_aff, *w_lat;
  bool s_aff [ N_PRE ], s [ N_POST ];
  float v [ N_POST ], syn_aff [ N_POST ];
} som_t;

void setInputs ( sfmt_t *rng, const float *image, som_t *som )
{
  for ( int32_t i = 0; i < N_PRE; i++ ) {
    som -> s_aff [ i ] = ( sfmt_genrand_real2 ( rng ) < 0.1 * image [ i ] );
  }
}

void initialize ( sfmt_t *rng, som_t *som )
{
  sfmt_init_gen_rand ( rng, 23 );

  som -> w_aff = ( float * ) calloc ( N_PRE * N_POST, sizeof ( float ) );
  for ( int32_t i = 0; i < N_POST; i++ ) {
    for ( int32_t j = 0; j < N_PRE; j++ ) {
      som -> w_aff [ j + N_PRE * i ] = sfmt_genrand_real2 ( rng );
    }
  }

  for ( int32_t i = 0; i < N_POST; i++ ) { som -> v [ i ] = V_INIT; }

  som -> w_lat = ( float * ) calloc ( N_POST * N_POST, sizeof ( float ) );
  for ( int32_t i = 0; i < N_POST; i++ ) {
    for ( int32_t j = 0; j < N_POST; j++ ) {
      int32_t ix = i / N_POST_X;
      int32_t iy = i % N_POST_X;
      int32_t jx = j / N_POST_X;
      int32_t jy = j % N_POST_X;
      float d2 = ( ix - jx ) * ( ix - jx ) + ( iy - jy ) * ( iy - jy );
      som -> w_lat [ j + N_POST * i ] = exp ( -d2 / ( 2 * SIGMA2 ) );
    }
  }
}

void finalize ( som_t *som ) {
  free ( som -> w_aff );
  free ( som -> w_lat );
}

void updateCellParameters ( som_t *som )
{
  for ( int32_t i = 0; i < N_POST; i++ ) {
    float r = 0.;
    for ( int32_t j = 0; j < N_PRE; j++ ) { r += som -> w_aff [ j + N_PRE * i ] * som -> s_aff [ j ]; }
    som -> syn_aff [ i ] = r;
  }

  for ( int32_t i = 0; i < N_POST; i++ ) {
    float dv = DT * ( - ( som -> v [ i ] - V_LEAK ) + G_SYN * som -> syn_aff [ i ] ) / TAU_M;
    som -> s [ i ] = ( som -> v [ i ] > THETA );
    som -> v [ i ] = som -> s [ i ] *  V_RESET + ( ! som -> s [ i ] ) * ( som -> v [ i ] + dv );
  }
}

int32_t findWinner ( sfmt_t *rng, bool *s )
{
  int32_t winners [ N_POST ], index = 0;

  for ( int32_t i = 0; i < N_POST; i++ ) {
    if ( s [ i ] ) { winners [ index ] = i; index ++; }
  }

  return ( ( index > 0 ) ? winners [ ( int32_t ) ( index * sfmt_genrand_real2 ( rng ) ) ] : -1 );
}

void updateSynapticWeights ( const int32_t winner, const float input [ ], som_t *som )
{
  int32_t pos_x = winner / N_POST_X;
  int32_t pos_y = winner % N_POST_X;

  for ( int32_t dx = -4; dx <= 4; dx++ ) {
    int32_t x = ( pos_x + dx + N_POST_X ) % N_POST_X; // periodic boundary condition
    for ( int32_t dy = -4; dy <= 4; dy++ ) {
      int32_t y = ( pos_y + dy + N_POST_Y ) % N_POST_Y; // periodic boundary condition
      int32_t target = y + N_POST_Y * x;
      float h = som -> w_lat [ target + N_POST * winner ];
      for ( int32_t i = 0; i < N_PRE; i++ ) {
	som -> w_aff [ i + N_PRE * target ] = som -> w_aff [ i + N_PRE * target ] + ETA * h * ( input [ i ] - som -> w_aff [ i + N_PRE * target ] );
      }
    }
  }
}

void generate_png ( float *w_aff, const char *filename )
{
  gdImagePtr im = gdImageCreate ( MNIST_IMAGE_ROW_SIZE * N_POST_X, MNIST_IMAGE_COL_SIZE * N_POST_Y );

  const int32_t n_grayscale = 256;
  int gray [ n_grayscale ];
  for ( int32_t i = 0; i < n_grayscale; i++ ) { gray [ i ] = gdImageColorAllocate ( im, i, i, i ); }

  for ( int32_t x = 0; x < N_POST_X; x++ ) {
    for ( int32_t y = 0; y < N_POST_Y; y++ ) {
      int32_t z = y + N_POST_Y * x;
      for ( int32_t i = 0; i < MNIST_IMAGE_ROW_SIZE; i++ ) {
        for ( int32_t j = 0; j < MNIST_IMAGE_COL_SIZE; j++ ) {
          int32_t k = j + MNIST_IMAGE_COL_SIZE * i;
          int32_t index = ( int32_t ) ( ( n_grayscale - 1 ) * w_aff [ k + MNIST_IMAGE_SIZE * z ] );
          gdImageSetPixel ( im, j + MNIST_IMAGE_COL_SIZE * y, i + MNIST_IMAGE_ROW_SIZE * x, gray [ index ] );
        }
      }
    }
  }

  {
    FILE *file = fopen ( filename, "wb" );
    gdImagePng ( im, file );
    fclose ( file );
  }

  gdImageDestroy ( im );
}
int main ( void )
{
  sfmt_t rng;
  som_t som;
  float **training_image, **test_image;
  uint32_t *training_label, *test_label;

  initialize ( &rng, &som );
  mnist_initialize ( &training_image, &training_label, &test_image, &test_label );

  for ( int32_t i = 0; i < MNIST_TRAINING_DATA_SIZE; i++ ) {
    fprintf ( stderr, "%d %d\n", i, training_label [ i ] );
    for ( int32_t nt = 0; nt < NT; nt++ ) {
      setInputs ( &rng, training_image [ i ], &som );
      updateCellParameters ( &som );
      int32_t winner = findWinner ( &rng, som . s );
      if ( winner != -1 ) { updateSynapticWeights ( winner, training_image [ i ], &som ); break; }
    }
  }

  generate_png ( som . w_aff, "w.png" );

  finalize ( &som );
  mnist_finalize ( training_image, training_label, test_image, test_label );
}
