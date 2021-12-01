#include <stdio.h>
#include <sys/time.h>

static struct timeval start, stop;

void timer_start(void) {
  gettimeofday(&start, NULL);
}

double timer_elapsed(void) {
  struct timeval elapsed;
  gettimeofday(&stop, NULL);
  timersub(&stop, &start, &elapsed);
  return ((elapsed.tv_sec*1000.0 + elapsed.tv_usec/1000.0)/1000.0);
}
