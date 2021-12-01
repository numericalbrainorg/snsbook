#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#define SEED ( 32 )

int main ( void )
{
  srand ( SEED );

  int32_t a = rand ( );
  double b = rand ( ) / ( double ) RAND_MAX;
}
