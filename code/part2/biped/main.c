#include"header.h"

void initialize(void)
{
  os_initialize("os.dat", "sp.dat");
  ms_initialize("ms.dat");
}
void finalize(void)
{
  os_finalize();
  ms_finalize();
}

int main(void)
{
  double feed[NOS] = {0.0};
  double y[NOS] = {0.0};
  double ankle[4];
  int n = 0;
  FILE *file;

  file = fopen("ankle.dat", "w");

  initialize();

  for(double t = 0.0; t < T; t += dt){
    os_print(t, y);
    ms_print(t, feed);

    if (n % 10000 == 0){
      ms_get_anklepos(ankle);
      fprintf(file, "%f %f %f %f %f\n",
	      t, ankle[XR], ankle[YR], ankle[XL], ankle[YL]);
    }
    ms_get_feed(feed);
    os_get_y(y);
    os_compute(t, feed);
    ms_compute(t, y);

    os_update(t);
    ms_update(t);
    n++;
  }

  finalize();
  fclose(file);

  return 0;
}

