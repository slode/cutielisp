#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "cutie.h"

static long int allocations = 0;

void* cutie_malloc(unsigned int sz) {
  void *p = malloc(sz);
  allocations++;
  //printf("Allocated %d bytes at %li. (%li)\n", sz, (long)p, allocations);
  return p;
}

void cutie_free(void *p) {
  free(p);
  allocations--;
//  printf("Freed memory at %li. (%li)\n", (long)p, allocations);
}

