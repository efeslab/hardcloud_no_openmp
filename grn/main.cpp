#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <x86intrin.h>
#include <time.h>
#include <hardcloud/hardcloud_app.h>

size_t cmdarg_get(const char *arg) {
    size_t l = strlen(arg);
    uint64_t n = atoll(arg);

	return n;
}

int main(int argc, char *argv[])
{
  int *results;
  uint64_t states;
  uint64_t out_size;

  if (argc < 2) {
    printf("Usage: %s states\n", argv[0]);
    return -1;
  }
  else {
    states = cmdarg_get(argv[1]);
    out_size = 2 * states;
  }

  HardcloudApp app;

  results = (int *) app.alloc_buffer(out_size * sizeof(int));

  printf("allocation done\n");
  getchar();

  struct timespec ts1, ts2;
  timespec_get(&ts1, TIME_UTC);
  app.run();
  timespec_get(&ts2, TIME_UTC);

  double t = (ts2.tv_sec*1000000 + ts2.tv_nsec/1000) - (ts1.tv_sec*1000000 + ts1.tv_nsec/1000);

  printf("time: %lf ms\n", t/1000);
  printf("throughput: %lf MB/s\n", 1.0*out_size*sizeof(int)/1024.0/1024.0/(t/1000000));

  app.delete_buffer(results);

  return 0;
}
