#include "header.h"

void mat_mul_mm(const int N, const int M, const int K,
	      const double A[], const double B[], double C[])
{
  for(int n = 1; n < N; n++){
    for(int m = 1; m < M; m++){
      C[m+M*n] = 0;
      for(int k = 1; k < K; k++){
	C[m+M*n] += A[k+K*n]*B[m+M*k];
      }
    }
  }
}
void mat_mul_mv(const int N, const int K,
		const double A[], const double U[], double V[])
{
  for(int n = 1; n < N; n++){
    V[n] = 0;
    for(int k = 1; k < K; k++){
      V[n] += A[k+K*n]*U[k];
    }
  }
}

void mat_add_vv(const int N,
		const double U[], const double V[], double W[])
{
  for(int n = 1; n < N; n++){
    W[n] = U[n] + V[n];
  }
}
void mat_sub_vv(const int N,
		const double U[], const double V[], double W[])
{
  for(int n = 1; n < N; n++){
    W[n] = U[n] - V[n];
  }
}

void mat_inv(const int N, const double A[], double Ainv[])
{
  double a[N][N], ainv[N][N], maxa;
  int maxi;

  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      a[i][j] = A[j+N*i];
    }
  }

  /* ---- 単位行列作成 ---------------------------- */
  for(int i = 0; i < N; i++){
    for(int j = 0; j < N; j++){
      if (i == j){
        ainv[i][j]=1.0;
      }else{
	ainv[i][j]=0.0;
      }
    }
  }

  for(int ipv = 1; ipv < N ; ipv++){

    /* ---- 最大値探索 ---------------------------- */
    maxi = ipv; 
    maxa = a[maxi][maxi];
    for(int i = ipv; i < N ; i++){
      if (fabs(a[i][ipv]) > maxa){
	maxa = fabs(a[i][ipv]);
	maxi = i;
      }
    }
    if (maxa == 0.0) { exit(1); } // guard.

    /* ---- 行の入れ替え -------------------------- */
    if (ipv != maxi){
      for(int i = 1; i < N; i++){
	double tmp;
	tmp = a[ipv][i];
	a[ipv][i] = a[maxi][i];
	a[maxi][i] = tmp;

	tmp = ainv[ipv][i];
	ainv[ipv][i] = ainv[maxi][i];
	ainv[maxi][i] = tmp;
      }
    }

    /* ---- 対角成分=1(ピボット行の処理) ---------- */
    double pivot = 1.0/a[ipv][ipv];
    for(int i = 1; i < N; i++){
      a[ipv][i] *= pivot;
      ainv[ipv][i] *= pivot;
    }

    /* ---- ピボット列=0(ピボット行以外の処理) ---- */
    for(int i = 1; i < N; i++){
      if (i != ipv){
	double tmp = a[i][ipv];
	for(int j = 1; j < N; j++){
	  a[i][j] -= tmp*a[ipv][j];
	  ainv[i][j] -= tmp*ainv[ipv][j];                	
	}
      }
    }
  }
  // copy
  for(int i = 1; i < N; i++){
    for(int j = 1; j < N; j++){
      Ainv[j+N*i] = ainv[i][j];
    }
  }
  // check
  for(int i = 1; i < N; i++){
    for(int j = 1; j < N; j++){
      double r = 0;
      for(int k = 1; k < N; k++){
	r += A[k+N*i]*Ainv[j+N*k];
      }
      if (i == j && ! (0.99 < r && r < 1.01)){
	fprintf(stderr, "mat.c:: err. %d %d %f\n", i, j, r);
      }
      if (i != j && ! (-0.01 < r && r < 0.01)){
	fprintf(stderr, "mat.c:: err. %d %d %f\n", i, j, r);
      }
    }
  }
}