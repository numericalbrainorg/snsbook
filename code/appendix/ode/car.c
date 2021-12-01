#include<stdio.h>

int main ( void )
{
  double t = 0; // 時刻: 最初は0秒から
  double x = 0; // 位置: 最初は原点 (0メートル) から
  double v = 1.0; // 速度: 1メートル/秒
  double dt = 1.0; // 時間の刻み幅: 1秒ずつ進める

  while ( t < 10.0 ) { // 10 秒間繰り返し

    printf ( "%f %f\n", t, x ); // 今の時刻と位置を表示

    x = x + v * dt; // 次の時刻の位置を計算
    t = t + dt; // 時間をdt秒進める
  }
}
