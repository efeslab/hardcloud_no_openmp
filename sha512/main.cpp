#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <x86intrin.h>
#include <hardcloud/hardcloud_app.h>

//#define NI 10000000*CL(1)/sizeof(uint64_t) // number of itens
//#define NJ 1*CL(1)/sizeof(uint64_t) // number of itens

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
  uint64_t* input;
  uint64_t* output;
  uint64_t ni, nj;

  if (argc < 2) {
	printf("Usage: %s data_size([P]age|[C]acheline)\n", argv[0]);
    return -1;
  }
  else {
    ni = cmdarg_getbytes(argv[1])/sizeof(uint64_t);
    nj = 1*CL(1)/sizeof(uint64_t);
  }

  HardcloudApp app;

  output = (uint64_t *) app.alloc_buffer(nj * sizeof(uint64_t));
  input = (uint64_t *) app.alloc_buffer(ni * sizeof(uint64_t));

  for (uint64_t i = 0; i < ni; i++)
  {
    input[i] = i;
  }

  printf("allocation done\n");

  struct timespec ts1, ts2;
  timespec_get(&ts1, TIME_UTC);
  app.run();
  timespec_get(&ts2, TIME_UTC);

  double t = (ts2.tv_sec*1000000 + ts2.tv_nsec/1000) - (ts1.tv_sec*1000000 + ts1.tv_nsec/1000);

  printf("time: %lf ms\n", t/1000);
  printf("throughput: %lf MB/s\n", 1.0*ni*sizeof(uint64_t)/1024.0/1024.0/(t/1000000));
  printf("hash value: ");
  for (uint64_t i = 0; i < 8; i++) {
      printf("%llx", output[i]);
  }
  printf("\n");

  app.delete_buffer(input);
  app.delete_buffer(output);

  return 0;
}

