#include"header.h"

#define TAU_SYN 0.1 
#define TAU_ADP 2.0 
#define TAU_OUT 0.01
#define E_LEAK ( -65.0 )
#define THRESHOLD ( -55.0 )
#define R_ADP 0.5
#define I_EXT 20.0
#define T_REF 0.001
static const double w[NOS][NOS] = { { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0, -1.9, -0.95, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0, -1.9,  0.0,  0.0, -0.95, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0, -0.95, 0.0,  0.0, -1.9,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0, -0.95,-1.9,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.9,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0, -0.95,-0.95, 0.0,  0.0, -1.9,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.9,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0,  0.0, -0.95,-0.95, 0.0,  0.0, -1.9,  0.0,  0.0,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.9,  0.0,  0.0},
                                    { 0.0, -0.95,-0.95, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.9,  0.0,  0.0,  0.0},
                                    { 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.9},
                                    { 0.0,  0.0,  0.0, -0.95,-0.95, 0.0,  0.0,  0.0,  0.0,  0.0,  0.0, -1.9,  0.0},
                                  };

const double tau_mbp[NOS] =  { 0.0, 0.01, 0.01, 0.01, 0.01, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005, 0.005};
static double v[NOS], dv[NOS], i_syn[NOS], i_adp[NOS], s[NOS], ts_last[NOS];
static FILE *file_os, *file_sp;
static int n;

extern double f(const double);

static double func(const double buf, const double s){
  return buf*exp(-dt/TAU_OUT) + s;
}

static void compute_rk4(const double feed[], double dv[], double t)
{
  double k1v[NOS], k2v[NOS], k3v[NOS], k4v[NOS], tmpv[NOS];

  for(int i = 1; i < NOS; i++){
    tmpv[i] = v[i];
  }

  for ( int i = 0; i < NOS; i++ ) {
    double r = 0;
    for ( int j = 0; j < NOS; j++ ) {
      r += w [ i ] [ j ] * s [ j ];
    }
    i_syn [ i ] = exp ( - dt / TAU_SYN ) * i_syn [ i ] + r + feed[i];
    i_adp [ i ] = exp ( - dt / TAU_ADP ) * i_adp [ i ] + s [ i ];
  }

  for(int i = 1; i < NOS; i++){
    k1v[i] = dt * ( - ( v [ i ] - E_LEAK ) + i_syn [ i ] - R_ADP * i_adp [ i ] + I_EXT ) / tau_mbp[i];//TAU_MBP;
    v[i] = tmpv[i]+0.5*dt*k1v[i];
  }
  for(int i = 1; i < NOS; i++){
    k2v[i] = dt * ( - ( v [ i ] - E_LEAK ) + i_syn [ i ] - R_ADP * i_adp [ i ] + I_EXT ) / tau_mbp[i];//TAU_MBP;
    v[i] = tmpv[i]+0.5*dt*k2v[i];
  }
  for(int i = 1; i < NOS; i++){
    k3v[i] = dt * ( - ( v [ i ] - E_LEAK ) + i_syn [ i ] - R_ADP * i_adp [ i ] + I_EXT ) / tau_mbp[i];//TAU_MBP;
    v[i] = tmpv[i]+dt*k3v[i];
  }
  for(int i = 1; i < NOS; i++){
    k4v[i] = dt * ( - ( v [ i ] - E_LEAK ) + i_syn [ i ] - R_ADP * i_adp [ i ] + I_EXT ) / tau_mbp[i];//TAU_MBP;
    v[i] = tmpv[i]+dt*k4v[i];
  }

  for(int i = 1; i < NOS; i++){
    v[i] = tmpv[i];
    dv[i] = (k1v[i]+2*k2v[i]+2*k3v[i]+k4v[i])/6.0;
    if ( v [ i ] > THRESHOLD && t > ts_last[i] + T_REF) {
      s [ i ] = 1;
      v [ i ] = E_LEAK;
      ts_last[i] = t;
    } else {
      s [ i ] = 0;
      v [ i ] += dv[i];
    }
  }
}

void os_initialize(const char *filename_os, const char *filename_sp)
{
  for(int i = 1; i < NOS; i++){
    v[i] = E_LEAK;
    i_syn[i] = i_adp[i] = s[i] = ts_last[i] = 0.0;
  }
  v[1] = E_LEAK - 5.0; v[3] = E_LEAK - 10.0;
  file_os = fopen(filename_os, "w");
  file_sp = fopen(filename_sp, "w");
  n = 0;
}
void os_finalize(void)
{
  fclose(file_os);
  fclose(file_sp);
}

void os_get_y(double buf[])
{
  for(int i = 1; i < NOS; i++){
    buf[i] = func(buf[i], s[i]);
  }
}

void os_compute(const double t, const double feed[])
{
  //compute_rk2(feed, dv, t);
  compute_rk4(feed, dv, t);
}

void os_update(const double t)
{
  for(int i = 1; i < NOS; i++){
    v[i] += dt*dv[i];
  }
  n++;
}
void os_print(const double t, const double y[NOS])
{
    fprintf(file_os, "%f ", t);
    fprintf(file_sp, "%f ", t);
    for(int i = 1; i < NOS; i++){
      fprintf(file_os, "%f ", y[i]);
      fprintf(file_sp, "%f ", v [ i ] - ( s [ i ] ? THRESHOLD : 0 ));
    }
    fprintf(file_os, "\n");
    fprintf(file_sp, "\n");
}
