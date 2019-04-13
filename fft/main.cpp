#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <x86intrin.h>
#include <hardcloud/hardcloud_app.h>

size_t cmdarg_getbytes(const char *arg) {
    size_t l = strlen(arg);
    uint64_t n = atoll(arg);
    switch (arg[l - 1]) {
        default:
        case 'p':
        case 'P': // unit is page
            return n * getpagesize();
        case 'c':
        case 'C': // unit is cache line
            return CL(n);
    }
}

int main(int argc, char *argv[])
{
  float *input;
  float *output;
  uint64_t num_words;

  if (argc < 2) {
    printf("Usage: %s data_size([P]age|[C]acheline)\n", argv[0]);
    return -1;
  }
  else {
    num_words = cmdarg_getbytes(argv[1])/sizeof(float);
  }

  HardcloudApp app;

  output = (float *) app.alloc_buffer(num_words * sizeof(float));
  input = (float *) app.alloc_buffer(num_words * sizeof(float));

  for (int k=0; k < num_words/2; k++)
  {
    input[2*k    ] = k;
    input[2*k + 1] = k;
  }

  unsigned long long t1 = __rdtsc();
  app.run();
  unsigned long long t2 = __rdtsc();

  printf("\nclk: %llu\n", t2 - t1);

  app.delete_buffer(input);
  app.delete_buffer(output);

  return 0;
}

