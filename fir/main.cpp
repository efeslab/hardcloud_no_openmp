#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
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
  uint8_t* data_in;
  uint8_t* data_out;
  uint64_t size;

  if (argc < 2) {
    printf("Usage: %s data_size([P]age|[C]acheline)\n", argv[0]);
    return -1;
  }
  else {
    size = cmdarg_getbytes(argv[1])/sizeof(uint8_t);
  }

  HardcloudApp app;

  data_out = (uint8_t *) app.alloc_buffer(size * sizeof(uint8_t));
  data_in = (uint8_t *) app.alloc_buffer(size * sizeof(uint8_t));

  for (uint64_t i = 0; i < size; i++)
  {
    data_in[i] = rand();
  }

  unsigned long long t1 = __rdtsc();
  app.run();
  unsigned long long t2 = __rdtsc();

  printf("\nclk: %llu\n", t2 - t1);

  app.delete_buffer(data_in);
  app.delete_buffer(data_out);

  return 0;
}

