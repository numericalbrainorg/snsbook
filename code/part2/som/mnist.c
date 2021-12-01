#include "mnist.h"

#define MNIST_DEBUG 0
#define MNIST_DEBUG2 0
#define MNIST_DEBUG3 0

static void mnist_read_image_file ( const char *filename, float ***data )
{
  FILE *file = fopen ( filename, "rb" );

  if ( file == NULL ) { fprintf ( stderr, "err fopen %s\n", filename ); exit ( 1 ); }
  
  { // get the magic number
    uint8_t buf [ 4 ];
    fread ( buf, sizeof ( uint8_t ), 4, file );
    int magic = buf [ 3 ] + 256 *( buf [ 2 ] + 256 *( buf [ 1 ] + 256 *( buf [ 0 ] )));
    if ( MNIST_DEBUG ) { printf ( "%d\n", magic ); }
    if ( MNIST_IMAGE_FILE_MAGIC != magic ) { fprintf ( stderr, "err magic\n" ); exit ( 1 ); }
  }

  uint32_t number_of_items;
  { // get the number of items
    uint8_t buf [ 4 ];
    fread ( buf, sizeof ( uint8_t ), 4, file );
    number_of_items = buf [ 3 ] + 256 *( buf [ 2 ] + 256 *( buf [ 1 ] + 256 *( buf [ 0 ] )));
    if ( MNIST_DEBUG ) { printf ( "%d\n", number_of_items ); }
  }

  { // skip 2 integers for row and column sizes
    uint8_t buf [ 8 ];
    fread ( buf, sizeof ( uint8_t ), 8, file );
  }

  { // read the data
    *data = ( float ** ) malloc ( number_of_items * sizeof ( float * ) );
    for ( int i = 0; i < number_of_items; i++ ) {
      (*data) [ i ] = ( float * ) malloc ( MNIST_IMAGE_SIZE * sizeof ( float ) );
    }

    for ( int i = 0; i < number_of_items; i++ ) {
      float r = 0.;
      for ( int j = 0; j < MNIST_IMAGE_SIZE; j++ ) {
	unsigned char buf;
	fread ( &buf, sizeof ( unsigned char ), 1, file );
	(*data) [ i ][ j ] = buf / 255.0;
	r += buf / 255.0;
	if ( MNIST_DEBUG && MNIST_DEBUG2 ) { printf ( "%f\n", ( *data ) [ i ][ j ] ); }
      }
      for ( int j = 0; j < MNIST_IMAGE_SIZE; j++ ) {
	//(*data) [ i ][ j ] *= ( 0.2*MNIST_IMAGE_SIZE / r );
      }
    }
  }
  
  fclose ( file );
}

static void mnist_read_label_file ( const char *filename, uint32_t **data )
{
  FILE *file = fopen ( filename, "rb" );

  if ( file == NULL ) { fprintf ( stderr, "err fopen %s\n", filename ); exit ( 1 ); }
  
  { // get the magic number
    uint8_t buf [ 4 ];
    fread ( buf, sizeof ( unsigned char ), 4, file );
    int magic = buf [ 3 ] + 256 *( buf [ 2 ] + 256 *( buf [ 1 ] + 256 *( buf [ 0 ] )));
    if ( MNIST_DEBUG ) { printf ( "%d\n", magic ); }
    if ( MNIST_LABEL_FILE_MAGIC != magic ) { fprintf ( stderr, "err magic\n" ); exit ( 1 ); }
  }

  uint32_t number_of_items;
  { // get the number of items
    uint8_t buf [ 4 ];
    fread ( buf, sizeof ( unsigned char ), 4, file );
    number_of_items = buf [ 3 ] + 256 *( buf [ 2 ] + 256 *( buf [ 1 ] + 256 *( buf [ 0 ] )));
    if ( MNIST_DEBUG ) { printf ( "%d\n", number_of_items ); }
  }

  { // read the data
    *data = ( uint32_t * ) calloc ( number_of_items, sizeof ( uint32_t ) );

    for ( uint32_t i = 0; i < number_of_items; i++ ) {
      uint8_t buf;
      fread ( &buf, sizeof ( uint8_t ), 1, file );
      (*data) [ i ] = buf;
      if ( MNIST_DEBUG && MNIST_DEBUG2 ) { printf ( "%d\n", ( *data ) [ i ] ); }
    }
  }
  
  fclose ( file );
}

void mnist_initialize ( float ***training_image, uint32_t **training_label, float ***test_image, uint32_t **test_label )
{
  mnist_read_image_file ( MNIST_TRAINING_IMAGE_FILE, training_image );
  mnist_read_label_file ( MNIST_TRAINING_LABEL_FILE, training_label );
  mnist_read_image_file ( MNIST_TEST_IMAGE_FILE, test_image );
  mnist_read_label_file ( MNIST_TEST_LABEL_FILE, test_label );
}

void mnist_finalize ( float **training_image, uint32_t *training_label, float **test_image, uint32_t *test_label )
{
  for ( uint32_t i = 0; i < MNIST_TRAINING_DATA_SIZE; i++ ) { free ( training_image [ i ] ); }
  free ( training_image );
  free ( training_label );
  
  for ( uint32_t i = 0; i < MNIST_TEST_DATA_SIZE; i++ ) { free ( test_image[ i ] ); }
  free ( test_image );
  free ( test_label );
}

void mnist_generate_png ( float **data, const uint32_t n, const char *filename )
{
  gdImagePtr im = gdImageCreate ( MNIST_IMAGE_ROW_SIZE, MNIST_IMAGE_COL_SIZE );

  const uint32_t n_grayscale = 256;
  int gray [ n_grayscale ];
  for ( uint32_t i = 0; i < n_grayscale; i++ ) { gray [ i ] = gdImageColorAllocate ( im, i, i, i ); }

  for ( uint32_t i = 0; i < MNIST_IMAGE_ROW_SIZE; i++ ) {
    for ( uint32_t j = 0; j < MNIST_IMAGE_COL_SIZE; j++ ) {
      uint32_t index = ( uint32_t ) ( ( n_grayscale - 1 ) * data [ n ][ j + MNIST_IMAGE_COL_SIZE * i ] );
      if ( MNIST_DEBUG && MNIST_DEBUG3 ) { printf ( "%d ", index ); }
      gdImageSetPixel ( im, j, i, gray [ index ] );
    }
  }
  
  {
    FILE *file = fopen ( filename, "wb" );
    gdImagePng ( im, file );
    fclose ( file );
  }

  gdImageDestroy ( im );
  
  return;
}

int local_main(void)
{
  float **training_image, **test_image;
  uint32_t *training_label, *test_label;

  mnist_initialize ( &training_image, &training_label, &test_image, &test_label );

  {  // Demo: generate 60 png files while printing corresponding labels
    for ( uint32_t i = 0; i < MNIST_TRAINING_DATA_SIZE; i += 1000 ) {
      char fn [ 1024 ];
      sprintf ( fn, "./png/%d.png", i );
      mnist_generate_png ( training_image, i, fn );
      printf ( "%d\n", training_label [ i ] );
    }
  }
  
  mnist_finalize ( training_image, training_label, test_image, test_label );

  return 0;
}

#if 0
int main(void)
{
  return local_main();
}
#endif
