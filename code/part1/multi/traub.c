#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <stdint.h>
#include <stdbool.h>

#define NC ( 19    ) // # compartments
#define DT ( 0.01  ) // 10 mu-sec
#define T  ( 200   ) // ms
#define NT ( 20000 ) // T / DT

#define Cm   ( 3.0 )  // micro F / cm^2
#define Ri   ( 0.1 )  // K Ohm-cm
#define Rm   ( 10.0 ) // K Ohm-cm^2 (unused)
#define Beta ( 0.075 )

#define V_LEAK ( -60.0 )
#define V_Na ( ( 115.0 + ( V_LEAK ) ) )
#define V_Ca ( ( 140.0 + ( V_LEAK ) ) )
#define V_K  ( ( -15.0 + ( V_LEAK ) ) )

enum { V, XI, M, S, N, C, A, H, R, B, Q, N_VARS };

static inline double pir2 ( const double rad ) { return M_PI * rad * rad; } // PI * r ^ 2

const double g_Na    [ NC ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 0.0, 15.0, 30.0, 15.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
const double g_K_DR  [ NC ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 20.0, 0.0, 5.0, 15.0, 5.0, 0.0, 20.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
const double g_K_A   [ NC ] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
const double g_K_C   [ NC ] = { 0.0, 5.0, 5.0, 10.0, 10.0, 10.0, 5.0, 20.0, 10.0, 20.0, 5.0, 15.0, 15.0, 15.0, 15.0, 15.0, 5.0, 5.0, 0.0 };
const double g_K_AHP [ NC ] = { 0.0, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.8, 0.0 };
const double g_Ca    [ NC ] = { 0.0, 5.0, 5.0, 12.0, 12.0, 12.0, 5.0, 8.0, 4.0, 8.0, 5.0, 17.0, 17.0, 17.0, 10.0, 10.0, 5.0, 5.0, 0.0 };
const double g_leak  [ NC ] = { 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1 };
const double phi     [ NC ] = { 7769, 7769, 7769, 7769.0, 7769.0, 7769.0, 7769.0, 34530.0, 17402.0, 26404.0, 5941.0, 5941.0, 5941.0, 5941.0, 5941.0, 5941.0, 5941.0, 5941.0, 5941.0 };
const double rad     [ NC ] = { 2.89e-4, 2.89e-4, 2.89e-4, 2.89e-4, 2.89e-4, 2.89e-4, 2.89e-4, 2.89e-4, 4.23e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4, 2.42e-4 };
const double len     [ NC ] = { 1.20e-2, 1.20e-2, 1.20e-2, 1.20e-2, 1.20e-2, 1.20e-2, 1.20e-2, 1.20e-2, 1.25e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2, 1.10e-2 };
const double area    [ NC ] = { 2.188e-5, 2.188e-5, 2.188e-5, 2.188e-5, 2.188e-5, 2.188e-5, 2.188e-5, 2.188e-5, 3.320e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5, 1.673e-5 };

static inline double alpha_m  ( const double v ) { return 0.32 * ( 13.1 - ( v - V_LEAK ) ) / ( exp ( ( 13.1 - ( v - V_LEAK ) ) / 4.0 ) - 1 ); }
static inline double alpha_s  ( const double v ) { return 1.6 / ( 1 + exp ( - 0.072 * ( ( v - V_LEAK ) - 65 ) ) ); }
static inline double alpha_n  ( const double v ) { return 0.016 * ( 35.1 - ( v - V_LEAK ) ) / ( exp ( ( 35.1 - ( v - V_LEAK ) ) / 5.0 ) - 1 ); }
static inline double alpha_c  ( const double v ) { return ( v <= ( 50 + V_LEAK ) ) ? exp ( ( ( v - V_LEAK ) - 10 ) / 11.0 - ( ( v - V_LEAK ) - 6.5 ) / 27.0 ) / 18.975 : 2 * exp ( - ( ( v - V_LEAK ) - 6.5 ) / 27.0 ); }
static inline double alpha_a  ( const double v ) { return 0.02 * ( 13.1 - ( v - V_LEAK ) ) / ( exp ( ( 13.1 - ( v - V_LEAK ) ) / 10.0 ) - 1 ); }
static inline double alpha_h  ( const double v ) { return 0.128 * exp ( ( 17 - ( v - V_LEAK ) ) / 18.0 ); }
static inline double alpha_r  ( const double v ) { return ( v <= ( 0 + V_LEAK ) ) ? 0.005 : exp ( - ( v - V_LEAK ) / 20.0 ) / 200.0; }
static inline double alpha_b  ( const double v ) { return 0.0016 * exp ( ( - 13 - ( v - V_LEAK ) ) / 18.0 ); }
static inline double alpha_q  ( const double x ) { return fmin ( ( 0.2e-4 ) * x, 0.01 ); }
static inline double beta_m   ( const double v ) { return 0.28 * ( ( v - V_LEAK ) - 40.1 ) / ( exp ( ( ( v - V_LEAK ) - 40.1 ) / 5.0 ) - 1 ); }
static inline double beta_s   ( const double v ) { return 0.02 * ( ( v - V_LEAK ) - 51.1 ) / ( exp ( ( ( v - V_LEAK ) - 51.1 ) / 5.0 ) - 1 ); }
static inline double beta_n   ( const double v ) { return 0.25 * exp ( ( 20 - ( v - V_LEAK ) ) / 40.0 ); }
static inline double beta_c   ( const double v ) { return ( v <= ( 50 + V_LEAK ) ) ? 2 * exp ( - ( ( v - V_LEAK ) - 6.5 ) / 27.0 ) - alpha_c ( v ) : 0; }
static inline double beta_a   ( const double v ) { return 0.0175 * ( ( v - V_LEAK ) - 40.1 ) / ( exp ( ( ( v - V_LEAK ) - 40.1 ) / 10.0 ) - 1 ); }
static inline double beta_h   ( const double v ) { return 4.0 / ( 1 + exp ( ( 40 - ( v - V_LEAK ) ) / 5.0 ) ); }
static inline double beta_r   ( const double v ) { return ( v <= ( 0 + V_LEAK ) ) ? 0 : 0.005 - alpha_r ( v ); }
static inline double beta_b   ( const double v ) { return 0.05 / ( 1 + exp ( ( 10.1 - ( v - V_LEAK ) ) / 5.0 ) ); }
static inline double beta_q   ( const double x ) { return 0.001; }

static inline double tau ( const int var, const double v )
{
  switch ( var ) {
  case M:
    return 1.0 / ( alpha_m ( v ) + beta_m ( v ) );
  case S:
    return 1.0 / ( alpha_s ( v ) + beta_s ( v ) );
  case N:
    return 1.0 / ( alpha_n ( v ) + beta_n ( v ) );
  case C:
    return 1.0 / ( alpha_c ( v ) + beta_c ( v ) );
  case A:
    return 1.0 / ( alpha_a ( v ) + beta_a ( v ) );
  case H:
    return 1.0 / ( alpha_h ( v ) + beta_h ( v ) );
  case R:
    return 1.0 / ( alpha_r ( v ) + beta_r ( v ) );
  case B:
    return 1.0 / ( alpha_b ( v ) + beta_b ( v ) );
  case Q:
    return 1.0 / ( alpha_q ( v ) + beta_q ( v ) );
  default:
    fprintf ( stderr, "Error: no such vars: %d\n", var );
    exit ( 1 );
  }
}

static inline double inf ( const int var, const double v )
{
  switch ( var ) {
  case M:
    return alpha_m ( v ) / ( alpha_m ( v ) + beta_m ( v ) );
  case S:
    return alpha_s ( v ) / ( alpha_s ( v ) + beta_s ( v ) );
  case N:
    return alpha_n ( v ) / ( alpha_n ( v ) + beta_n ( v ) );
  case C:
    return alpha_c ( v ) / ( alpha_c ( v ) + beta_c ( v ) );
  case A:
    return alpha_a ( v ) / ( alpha_a ( v ) + beta_a ( v ) );
  case H:
    return alpha_h ( v ) / ( alpha_h ( v ) + beta_h ( v ) );
  case R:
    return alpha_r ( v ) / ( alpha_r ( v ) + beta_r ( v ) );
  case B:
    return alpha_b ( v ) / ( alpha_b ( v ) + beta_b ( v ) );
  case Q:
    return alpha_q ( v ) / ( alpha_q ( v ) + beta_q ( v ) );
  default:
    fprintf ( stderr, "Error: no such vars: %d\n", var );
    exit ( 1 );
  }
}

void initialize ( double var [ N_VARS ] [ NC ], double I_inj [ NC ], double g_comp [ NC ] [ 2 ] )
{
  // Membrane potential
  for ( int32_t i = 0; i < NC; i++ ) {
    var [ V ] [ i ] = V_LEAK;
    I_inj [ i ] = 0.;
  }

  // Gate variables
  for ( int32_t i = 0; i < NC; i++ ) {
    for ( int32_t j = 2; j < N_VARS - 1; j++ ) {
      var [ j ] [ i ] = inf ( j, var [ V ] [ i ] );
    }
  }
  for ( int32_t i = 0; i < NC; i++ ) {
    double i_Ca = g_Ca [ i ] * area [ i ] * var [ S ] [ i ] * var [ S ] [ i ] * var [ R ] [ i ] * ( var [ V ] [ i ] - V_Ca );
    var [ XI ] [ i ] = - i_Ca * phi [ i ] / Beta;
    var [ Q  ] [ i ] = inf ( Q, var [ XI ] [ i ] );
  }

  // Conductance between compartments
  for ( int32_t i = 0; i < NC; i++ ) {
    g_comp [ i ] [ 0 ] = ( i == 0      ) ? 0. : 2.0 / ( ( Ri * len [ i - 1 ] ) / pir2 ( rad [ i - 1 ] ) + ( Ri * len [ i     ] ) / pir2 ( rad [ i     ] ) );
    g_comp [ i ] [ 1 ] = ( i == NC - 1 ) ? 0. : 2.0 / ( ( Ri * len [ i     ] ) / pir2 ( rad [ i     ] ) + ( Ri * len [ i + 1 ] ) / pir2 ( rad [ i + 1 ] ) );
  }
}

void solve_euler ( double var [ N_VARS ] [ NC ], double i_inj [ NC ], double g_comp [ NC ] [ 2 ] )
{
  double dvar [ N_VARS ] [ NC ];
  double i_ion [ NC ], i_comp [ NC ];

  for ( int32_t i = 0; i < NC; i++ ) {
    double *v = var [ V ];
    for ( int32_t j = 2; j < N_VARS - 1; j++ ) {
      dvar [ j ] [ i ] = ( DT / tau ( j, v [ i ] ) ) * ( - var [ j ] [ i ] + inf ( j, v [ i ] ) );
    }
    dvar [ Q ] [ i ] = ( DT / tau ( Q, var [ XI ] [ i ] ) ) * ( - var [ Q ] [ i ] + inf ( Q, var [ XI ] [ i ] ) );

    i_ion [ i ] = ( g_leak    [ i ] * ( v [ i ] - V_LEAK )
                    + g_Na    [ i ] * var [ M ] [ i ] * var [ M ] [ i ] * var [ H ] [ i ] * ( v [ i ] - V_Na )
		    + g_Ca    [ i ] * var [ S ] [ i ] * var [ S ] [ i ] * var [ R ] [ i ] * ( v [ i ] - V_Ca )
		    + g_K_DR  [ i ] * var [ N ] [ i ] * ( v [ i ] - V_K )
		    + g_K_A   [ i ] * var [ A ] [ i ] * var [ B ] [ i ] * ( v [ i ] - V_K )
		    + g_K_AHP [ i ] * var [ Q ] [ i ] * ( v [ i ] - V_K )
		    + g_K_C   [ i ] * var [ C ] [ i ] * fmin ( 1, var [ XI ] [ i ] / 250.0 ) * ( v [ i ] - V_K ) );

    i_comp [ i ] = (   ( ( i == 0      ) ? 0. : g_comp [ i ] [0] * ( v [ i - 1 ] - v [ i ] ) / area [ i ] )
		     + ( ( i == NC - 1 ) ? 0. : g_comp [ i ] [1] * ( v [ i + 1 ] - v [ i ] ) / area [ i ] ) );
    
    dvar [ V ] [ i ] = ( DT / Cm ) * ( - i_ion [ i ] + i_comp [ i ] + i_inj [ i ] );

    double i_Ca = g_Ca [ i ] * area [ i ] * var [ S ] [ i ] * var [ S ] [ i ] * var [ R ] [ i ] * ( var [ V ] [ i ] - V_Ca );
    dvar [ XI ] [ i ] = DT * ( - phi [ i ] * i_Ca - Beta * var [ XI ] [ i ] );
  }

  for ( int32_t i = 0; i < NC; i++ ) {
    for ( int32_t j = 0; j < N_VARS; j++ ) {
      var [ j ] [ i ] += dvar [ j ] [ i ];
    }
  }
}

int main(void)
{
  double var [ N_VARS ] [ NC ], i_inj [ NC ], g_comp [ NC ] [ 2 ];

  initialize ( var, i_inj, g_comp );

  for ( int32_t nt = 0; nt < NT; nt++ ) {
    double t = DT * nt;

    printf ( "%f ", t ); for ( int32_t i = 0; i < NC; i++ ) { printf ( "%f%s", var [ V ] [ i ], ( i == NC - 1 ) ? "\n" : " " ); }

    i_inj [ 8 ] = 1.e-4 / area [ 8 ];
    //i_inj [ 8 ] = (  1000 <= t && t < 1001 ) ? 1.e-2 / area [ 8 ] : 0.;

    solve_euler ( var, i_inj, g_comp );
  }
}
