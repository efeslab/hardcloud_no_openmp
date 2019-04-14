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

  getchar();

  struct timespec ts1, ts2;
  timespec_get(&ts1, TIME_UTC);
  app.run();
  timespec_get(&ts2, TIME_UTC);

  double t = (ts2.tv_sec*1000000 + ts2.tv_nsec/1000) - (ts1.tv_sec*1000000 + ts1.tv_nsec/1000);

  printf("time: %lf ms\n", t/1000);
  printf("throughput: %lf MB/s\n", 1.0*num_words*sizeof(float)/1024.0/1024.0/(t/1000000));

  app.delete_buffer(input);
  app.delete_buffer(output);

  return 0;
}

