#include "header.h"

#define m 48.0
#define m1 7.0
#define m2 4.0
#define l1 0.5
#define l2 0.6

#define b1 10.0
#define b2 10.0
#define bk 1000.0

#define bg 1000.0
#define kk 10000.0
#define kg 10000.0
#define g 9.8

#define phf 162.675
#define phe 864.025
#define pkf 50.8500
#define pke 32.4750
#define paf 1472.00
#define pae 618.375

#define a1 0.19240
#define a2 0.00090
#define a3 0.15190
#define a4 0.05540
#define a5 0.11790
#define a6 0.01590
#define a7 0.19630
#define a8 0.03840
//
#define pid(i,j) ((j)+NCF*(i))
#define cid(i,j) ((j)+NMS*(i))

static double i1, i2, xr0, yr0, xl0, yl0;
static double z[NMS], x[NMS], dz[NMS], dx[NMS];
static double q[NMS], p[NMS*NCF], d[NCF], c[NCF*NMS];

static int n;
static FILE *file;

extern void mat_mul_mm(const int, const int, const int,
		       const double [], const double [], double []);
extern void mat_mul_mv(const int, const int,
		       const double [], const double [], double []);
extern void mat_inv(const int, const double [], double []);
extern void mat_add_vv(const int, const double [], const double [], double []);
extern void mat_sub_vv(const int, const double [], const double [], double []);

extern double f(const double);
extern double h(const double);

static double xr(void) { return  x[9] + (l2/2.0)*cos(x[11]); }
static double yr(void) { return x[10] - (l2/2.0)*sin(x[11]); }
static double xl(void) { return x[12] + (l2/2.0)*cos(x[14]); }
static double yl(void) { return x[13] - (l2/2.0)*sin(x[14]); }
static double dxr(void) { return z[9]  - (l2/2.0)*z[11]*sin(x[11]); }
static double dyr(void) { return z[10] - (l2/2.0)*z[11]*cos(x[11]); }
static double dxl(void) { return z[12] - (l2/2.0)*z[14]*sin(x[14]); }
static double dyl(void) { return z[13] - (l2/2.0)*z[14]*cos(x[14]); }
static double fg1(void) { return (yr() < 0.01) ? (-kg*(xr() - xr()) - bg*dxr()) : 0; }
static double fg2(void) { return (yr() < 0.01) ? (-kg*(yr() - 0.01) + bg*f(-dyr())) : 0; }
static double fg3(void) { return (yl() < 0.01) ? (-kg*(xl() - xl()) - bg*dxl()) : 0; }
static double fg4(void) { return (yl() < 0.01) ? (-kg*(yl() - 0.01) + bg*f(-dyl())) : 0; }

static void initialize_const(void)
{
  i1 = m1*pow(l1,2.0)/12.0;
  i2 = m2*pow(l2,2.0)/12.0;

  x[0] = 0.0;
  x[1] = 0.0;
  x[2] = 1.09;

  x[5] = 0.45*M_PI;
  x[11] = 0.45*M_PI;
  x[8] = 0.57*M_PI;
  x[14] = 0.57*M_PI;

  x[3] = x[1]+(l1/2.0)*cos(x[5]);
  x[4] = x[2]-(l2/2.0)*sin(x[5]);
  x[6] = x[1]+(l1/2.0)*cos(x[8]);
  x[7] = x[2]-(l1/2.0)*sin(x[8]);
  x[9] = l1*cos(x[5])+(l2/2.0)*cos(x[11]);
  x[10] = x[2]-l1*sin(x[5])-(l2/2.0)*sin(x[11]);
  x[12] = l1*cos(x[8])+(l2/2.0)*cos(x[14]);
  x[13] = x[2]-l1*sin(x[8])-(l2/2.0)*sin(x[14]);

  for(int i = 1; i < NMS; i++){
    z[i] = 0;
  }

  xr0 = xr();
  yr0 = yr();
  xl0 = xl();
  yl0 = yl();
}
static void update_q(const double y[NOS], double q[NMS])
{
  double tr[NTR];

  tr[1] = (phe*y[2] - phf*y[1]);
  tr[2] = (phe*y[4] - phf*y[3]);
  tr[3] = (pke*y[6] - pkf*y[5]);
  tr[4] = (pke*y[8] - pkf*y[7]);
  tr[5] = (pae*y[10] - paf*y[9])*h(fg2());
  tr[6] = (pae*y[12] - paf*y[11])*h(fg4());

  q[5] = (-b1*fabs(x[5]-M_PI/2.0)*z[5]-(b2+bk*f(x[5]-x[11]))*(z[5]-z[11])-kk*h(x[5]-x[11])+tr[1]+tr[3])/i1;
  q[8] = (-b1*fabs(x[8]-M_PI/2.0)*z[8]-(b2+bk*f(x[8]-x[14]))*(z[8]-z[14])-kk*h(x[8]-x[14])+tr[2]+tr[4])/i1;
  q[9] = fg1()/m2;
  q[10] = (fg2()-m2*g)/m2;
  q[11] = (-fg1()*(l2/2.0)*sin(x[11])-fg2()*(l2/2.0)*cos(x[11])-(b2+bk*f(x[5]-x[11]))*(z[11]-z[5])+kk*h(x[5]-x[11])-tr[3]-tr[5])/i2;
  q[12] = fg3()/m2;
  q[13] = (fg4()-m2*g)/m2;
  q[14] = (-fg3()*(l2/2.0)*sin(x[14])-fg4()*(l2/2.0)*cos(x[14])-(b2+bk*f(x[8]-x[14]))*(z[14]-z[8])+kk*h(x[8]-x[14])-tr[4]-tr[6])/i2;
}
static void update_p(double p[NMS*NCF])
{
  p[pid(5,1)] = p[pid(5,5)] = -(l1/2.0)*sin(x[5])/i1;
  p[pid(5,2)] = p[pid(5,6)] = -(l1/2.0)*cos(x[5])/i1;
  p[pid(8,3)] = p[pid(8,7)] = -(l1/2.0)*sin(x[8])/i1;
  p[pid(8,4)] = p[pid(8,8)] = -(l1/2.0)*cos(x[8])/i1;
  p[pid(11,5)] = -(l2/2.0)*sin(x[11])/i2;
  p[pid(11,6)] = -(l2/2.0)*cos(x[11])/i2;
  p[pid(14,7)] = -(l2/2.0)*sin(x[14])/i2;
  p[pid(14,8)] = -(l2/2.0)*cos(x[14])/i2;
}
static void update_d(double d[NCF])
{
  d[1] = (l1/2.0)*cos(x[5])*pow(z[5],2.0);
  d[2] =-(l1/2.0)*sin(x[5])*pow(z[5],2.0);
  d[3] =(l1/2.0)*cos(x[8])*pow(z[8],2.0);
  d[4] =-(l1/2.0)*sin(x[8])*pow(z[8],2.0);
  d[5] =(l1/2.0)*cos(x[5])*pow(z[5],2.0) + (l2/2.0)*cos(x[11])*pow(z[11],2.0);
  d[6] =-(l1/2.0)*sin(x[5])*pow(z[5],2.0) - (l2/2.0)*sin(x[11])*pow(z[11],2.0);
  d[7] =(l1/2.0)*cos(x[8])*pow(z[8],2.0) + (l2/2.0)*cos(x[14])*pow(z[14],2.0);
  d[8] =-(l1/2.0)*sin(x[8])*pow(z[8],2.0) - (l2/2.0)*sin(x[14])*pow(z[14],2.0);
}
static void update_c(double c[NCF*NMS])
{
  c[cid(1,5)] = -(l1/2.0)*sin(x[5]);
  c[cid(2,5)] = -(l1/2.0)*cos(x[5]);
  c[cid(3,8)] = -(l1/2.0)*sin(x[8]);
  c[cid(4,8)] = -(l1/2.0)*cos(x[8]);
  c[cid(5,5)] = -(l1/2.0)*sin(x[5]);
  c[cid(5,11)] = -(l2/2.0)*sin(x[11]);
  c[cid(6,5)] = -(l1/2.0)*cos(x[5]);
  c[cid(6,11)] = -(l2/2.0)*cos(x[11]);
  c[cid(7,8)] = -(l1/2.0)*sin(x[8]);
  c[cid(7,14)] = -(l2/2.0)*sin(x[14]);
  c[cid(8,8)] = -(l1/2.0)*cos(x[8]);
  c[cid(8,14)] = -(l2/2.0)*cos(x[14]);
}
static void initialize_p(double p[NMS*NCF])
{
  for(int i = 1; i < NMS; i++){
    for(int j = 1; j < NCF; j++){
      p[pid(i,j)] = 0;
    }
  }
  p[pid(1,1)] = p[pid(1,3)] = p[pid(2,2)] = p[pid(2,4)] = 1.0/m;
  p[pid(3,5)] = p[pid(4,6)] = p[pid(6,7)] = p[pid(7,8)] = 1.0/m1; 
  p[pid(3,1)] = p[pid(4,2)] = p[pid(6,3)] = p[pid(7,4)] = -1.0/m1;
  p[pid(9,5)] = p[pid(10,6)] = p[pid(12,7)] = p[pid(13,8)] = -1.0/m2;  

  update_p(p);
}
static void initialize_d(double d[NCF])
{
  update_d(d);
}
static void initialize_c(double c[NCF*NMS])
{
  for(int i = 0; i < NCF; i++){
    for(int j = 0; j < NMS; j++){
      c[cid(i,j)] = 0;
    }
  }
  c[cid(1,1)] = c[cid(2,2)] = c[cid(3,1)] = c[cid(4,2)] = 1.0;
  c[cid(5,3)] = c[cid(6,4)] = c[cid(7,6)] = c[cid(8,7)] = 1.0;
  c[cid(1,3)] = c[cid(2,4)] = c[cid(3,6)] = c[cid(4,7)] = -1.0;
  c[cid(5,9)] = c[cid(6,10)] = c[cid(7,12)] = c[cid(8,13)] = -1.0;

  update_c(c);
}
static void initialize_q(double q[NMS])
{
  double y[NOS] = {0.0};

  q[0] = 0.0;
  q[1] = 0.0;
  q[2] = -m*g/m;
  q[3] = 0.0;
  q[4] = -m1*g/m1;
  q[6] = 0.0;
  q[7] = -m1*g/m1;

  os_get_y(y);
  update_q(y, q);
}

static void compute_rk4(const double y[NOS], double dx[NMS], double dz[NMS])
{
  double cp[NCF*NCF], cpinv[NCF*NCF], cq[NCF], dcq[NCF], cpdcq[NCF];
  double pcpdcq[NMS*NCF];
  double k1z[NMS], k2z[NMS], k3z[NMS], k4z[NMS], tmpz[NMS]; 
  double k1x[NMS], k2x[NMS], k3x[NMS], k4x[NMS], tmpx[NMS];

  for(int i = 1; i < NMS; i++){
    tmpx[i] = x[i];
    tmpz[i] = z[i];
  }

  for(int i = 1; i < NMS; i++){
    k1x[i] = z[i];
  }
  update_p(p);
  update_c(c);
  update_d(d);
  update_q(y, q);
  mat_mul_mm(NCF, NCF, NMS, c, p, cp);
  mat_inv(NCF, cp, cpinv);
  mat_mul_mv(NCF, NMS, c, q, cq);
  mat_sub_vv(NCF, d, cq, dcq);
  mat_mul_mv(NCF, NCF, cpinv, dcq, cpdcq);
  mat_mul_mv(NMS, NCF, p, cpdcq, pcpdcq);
  mat_add_vv(NMS, pcpdcq, q, k1z);
  for(int i = 1; i < NMS; i++){
    x[i] = tmpx[i] + 0.5*dt*k1x[i];
    z[i] = tmpz[i] + 0.5*dt*k1z[i];
  }

  for(int i = 1; i < NMS; i++){
    k2x[i] = z[i];
  }
  update_p(p);
  update_c(c);
  update_d(d);
  update_q(y, q);
  mat_mul_mm(NCF, NCF, NMS, c, p, cp);
  mat_inv(NCF, cp, cpinv);
  mat_mul_mv(NCF, NMS, c, q, cq);
  mat_sub_vv(NCF, d, cq, dcq);
  mat_mul_mv(NCF, NCF, cpinv, dcq, cpdcq);
  mat_mul_mv(NMS, NCF, p, cpdcq, pcpdcq);
  mat_add_vv(NMS, pcpdcq, q, k2z);
  for(int i = 1; i < NMS; i++){
    x[i] = tmpx[i] + 0.5*dt*k2x[i];
    z[i] = tmpz[i] + 0.5*dt*k2z[i];
  }

  for(int i = 1; i < NMS; i++){
    k3x[i] = z[i];
  }
  update_p(p);
  update_c(c);
  update_d(d);
  update_q(y, q);
  mat_mul_mm(NCF, NCF, NMS, c, p, cp);
  mat_inv(NCF, cp, cpinv);
  mat_mul_mv(NCF, NMS, c, q, cq);
  mat_sub_vv(NCF, d, cq, dcq);
  mat_mul_mv(NCF, NCF, cpinv, dcq, cpdcq);
  mat_mul_mv(NMS, NCF, p, cpdcq, pcpdcq);
  mat_add_vv(NMS, pcpdcq, q, k3z);
  for(int i = 1; i < NMS; i++){
    x[i] = tmpx[i] + dt*k3x[i];
    z[i] = tmpz[i] + dt*k3z[i];
  }

  for(int i = 1; i < NMS; i++){
    k4x[i] = z[i];
  }
  update_p(p);
  update_c(c);
  update_d(d);
  update_q(y, q);
  mat_mul_mm(NCF, NCF, NMS, c, p, cp);
  mat_inv(NCF, cp, cpinv);
  mat_mul_mv(NCF, NMS, c, q, cq);
  mat_sub_vv(NCF, d, cq, dcq);
  mat_mul_mv(NCF, NCF, cpinv, dcq, cpdcq);
  mat_mul_mv(NMS, NCF, p, cpdcq, pcpdcq);
  mat_add_vv(NMS, pcpdcq, q, k4z);

  for(int i = 1; i < NMS; i++){
    x[i] = tmpx[i];
    z[i] = tmpz[i];
    dx[i] = (k1x[i]+2*k2x[i]+2*k3x[i]+k4x[i])/6.0;
    dz[i] = (k1z[i]+2*k2z[i]+2*k3z[i]+k4z[i])/6.0;
  }

}

void ms_initialize(const char *filename)
{
  // The order is essential.
  initialize_const();
  initialize_p(p);
  initialize_c(c);
  initialize_d(d);
  initialize_q(q);

  file = fopen(filename, "w");
  n = 0;
}

void ms_get_feed(double feed[NOS])
{
  if (n % 100 == 0){  
    feed[1] = a1*(x[5] - M_PI/2.0) - a2*(x[8] - M_PI/2.0) + a3*(x[11] - M_PI/2.0)*h(fg2()) + a4*h(fg4());
    feed[2] = a1*(M_PI/2.0 - x[5]) - a2*(M_PI/2.0 - x[8]) + a3*(M_PI/2.0 - x[11])*h(fg2()) - a4*h(fg4());
    feed[3] = a1*(x[8] - M_PI/2.0) - a2*(x[5] - M_PI/2.0) + a3*(x[14] - M_PI/2.0)*h(fg4()) + a4*h(fg2());
    feed[4] = a1*(M_PI/2.0 - x[8]) - a2*(M_PI/2.0 - x[5]) + a3*(M_PI/2.0 - x[14])*h(fg4()) - a4*h(fg2());
    feed[5] = a5*(M_PI/2.0 - x[14])*h(fg4());
    feed[6] = a5*(x[14] - M_PI/2.0)*h(fg4());
    feed[7] = a5*(M_PI/2.0 - x[11])*h(fg2());
    feed[8] = a5*(x[11] - M_PI/2.0)*h(fg2());
    feed[9] = a6*(M_PI/2.0 - x[11])*h(fg2()) + a7*(M_PI/2.0 - x[14])*h(fg4()) - a8*z[11]*h(fg2());
    feed[10] = a6*(x[11] - M_PI/2.0)*h(fg2()) + a7*(x[14] - M_PI/2.0)*h(fg4()) + a8*z[11]*h(fg2());
    feed[11] = a6*(M_PI/2.0 - x[14])*h(fg4()) + a7*(M_PI/2.0 - x[11])*h(fg2()) - a8*z[14]*h(fg4());
    feed[12] = a6*(x[14] - M_PI/2.0)*h(fg4()) + a7*(x[11] - M_PI/2.0)*h(fg2()) + a8*z[14]*h(fg4());
  } else {
    for(int i = 1; i < NOS; i++){
      feed[i] = 0.0;
    }
  }
}

void ms_get_anklepos(double ankle[])
{
  ankle[XR] = xr();
  ankle[YR] = yr();
  ankle[XL] = xl();
  ankle[YL] = yl();
}

void ms_compute(const double t, const double y[NOS])
{
  compute_rk4(y, dx, dz);
}

void ms_update(const double t)
{
  for(int i = 1; i < NMS; i++){
    x[i] += dt*dx[i];
    z[i] += dt*dz[i];
  }
  n++;
}

void ms_print(const double t, const double feed[])
{
  if (n % 1000 == 0){
    fprintf(file, "%f ", t);
    for(int i = 1; i < NMS; i++){
      fprintf(file, "%f ", x[i]);
    }
    fprintf(file, "\n");
  }
}

void ms_finalize(void)
{
  fclose(file);
}
