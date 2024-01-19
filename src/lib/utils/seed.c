#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void seed_set(unsigned int seed) {
  srand(seed);
  printf("Seed set to %u\n", seed);
}

void seed_set_to_time() {
  unsigned int seed = (unsigned int)time(NULL);
  seed_set(seed);
}
