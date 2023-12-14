#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "./lib/interval/interval.h"
#include "./lib/mapping/mapping.h"
#include "./lib/solvers/solvers.h"
#include "main.h"

int main() {
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);

  struct Interval *interval =
      interval_factory.generate_interval_by_imbalance(20, 16);
  interval_print(interval);

  struct Mapping *mapping = karp_li_solver.solve(interval);
  mapping_print(mapping);

  interval_free(interval);
  mapping_free(mapping);
  return 0;
}
