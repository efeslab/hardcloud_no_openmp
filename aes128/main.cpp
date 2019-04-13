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
  uint64_t* key;
  uint64_t* pt;
  uint64_t* ct;
  uint64_t num_words;

  if (argc < 2) {
	printf("Usage: %s data_size([P]age|[C]acheline)\n", argv[0]);
    return -1;
  }
  else {
    num_words = cmdarg_getbytes(argv[1])/sizeof(uint64_t);
  }

  HardcloudApp app;

  ct = (uint64_t *) app.alloc_buffer(num_words * sizeof(uint64_t));
  key = (uint64_t *) app.alloc_buffer(8 * sizeof(uint64_t));
  pt = (uint64_t *) app.alloc_buffer(num_words * sizeof(uint64_t));

  for (uint64_t i = 0; i < 8; i++)
  {
    key[i] = (uint64_t)rand()<<32 & rand();
  }

  for (uint64_t i = 0; i < num_words; i++)
  {
    pt[i] = (uint64_t)rand()<< 32 & rand();
  }

  unsigned int dummy;
  unsigned long long t1 = __rdtsc();

  app.run();

  unsigned long long t2 = __rdtsc();

//  for (uint64_t i = 0; i < NUM_WORDS; i += 2)
//  {
//    printf("ct[%lu] = %016lx%016lx\n", i/2, ct[i + 1], ct[i]);
//  }

  printf("Time: %llu\n", t2-t1);

  app.delete_buffer(pt);
  app.delete_buffer(key);
  app.delete_buffer(ct);

  return 0;
}

