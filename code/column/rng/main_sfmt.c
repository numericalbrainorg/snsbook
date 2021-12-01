#include <stdio.h>
#include <stdint.h>
#include <SFMT.h>

#define SEED ( 32 )

extern void sfmt_init_gen_rand ( sfmt_t *, uint32_t );
extern uint32_t sfmt_rangend_uint32 ( sfmt_t * );
extern double sfmt_genrand_real1 ( sfmt_t * );

int main ( void )
{
  sfmt_t rng;
  sfmt_init_gen_rand ( &rng, SEED );

  uint32_t a = sfmt_genrand_uint32 ( &rng );
  double b = sfmt_genrand_real1 ( &rng );
}
