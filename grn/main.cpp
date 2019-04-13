#include <stdlib.h>
#include <stdio.h>
#include <cmath>
#include <x86intrin.h>
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

  unsigned long long t1 = __rdtsc();
  results = (int *) app.alloc_buffer(out_size * sizeof(int));
  unsigned long long t2 = __rdtsc();

  printf("\nclk: %llu\n", t2 - t1);

  app.delete_buffer(results);

  return 0;
}
