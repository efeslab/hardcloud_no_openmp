#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
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
  uint32_t* data_in;
  uint32_t* data_out;
  uint64_t ni, nj;

  if (argc < 2) {
    printf("Usage: %s data_size([P]age|[C]acheline)\n", argv[0]);
    return -1;
  }
  else {
    ni = cmdarg_getbytes(argv[1])/sizeof(uint32_t);
    nj = ni/2;
  }

  HardcloudApp app;

  data_out = (uint32_t *) app.alloc_buffer(nj * sizeof(uint32_t));
  data_in = (uint32_t *) app.alloc_buffer(ni * sizeof(uint32_t));

  for (uint64_t i = 0; i < ni; i++)
  {
    data_in[i] = i;
  }

  printf("allocation done\n");
  getchar();

  struct timespec ts1, ts2;
  timespec_get(&ts1, TIME_UTC);
  app.run();
  timespec_get(&ts2, TIME_UTC);

  double t = (ts2.tv_sec*1000000 + ts2.tv_nsec/1000) - (ts1.tv_sec*1000000 + ts1.tv_nsec/1000);

  printf("time: %lf ms\n", t/1000);
  printf("throughput: %lf MB/s\n", 1.0*ni*sizeof(uint32_t)/1024.0/1024.0/(t/1000000));

  app.delete_buffer(data_in);
  app.delete_buffer(data_out);

  return 0;
}

