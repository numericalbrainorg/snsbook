#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <gd.h>

#define MNIST_TRAINING_IMAGE_FILE "./mnist/train-images-idx3-ubyte"
#define MNIST_TRAINING_LABEL_FILE "./mnist/train-labels-idx1-ubyte"
#define MNIST_TEST_IMAGE_FILE "./mnist/t10k-images-idx3-ubyte"
#define MNIST_TEST_LABEL_FILE "./mnist/t10k-labels-idx1-ubyte"

#define MNIST_IMAGE_FILE_MAGIC ( 2051 )
#define MNIST_LABEL_FILE_MAGIC ( 2049 )

#define MNIST_TRAINING_DATA_SIZE ( 60000 )
#define MNIST_TEST_DATA_SIZE ( 10000 )
#define MNIST_IMAGE_ROW_SIZE ( 28 )
#define MNIST_IMAGE_COL_SIZE ( 28 )
#define MNIST_IMAGE_SIZE ( ( MNIST_IMAGE_ROW_SIZE ) * ( MNIST_IMAGE_COL_SIZE ) )
#define MNIST_LABEL_SIZE ( 10 )

void mnist_initialize ( float ***, uint32_t **, float ***, uint32_t ** );
void mnist_finalize ( float **, uint32_t *, float **, uint32_t * );
void mnist_generate_png ( float **, const uint32_t, const char * );
