#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "interval/interval.h"
#include "main.h"
#include "mapping/mapping.h"
#include "solvers/solvers.h"

int main() {
  unsigned int seed = (unsigned int)time(NULL);
  srand(seed);
  printf("Seed set to %u\n", seed);
  struct Interval *interval = interval_factory.generate_interval(64, 18, 24);

  struct Mapping *mapping = iterative_solver.solve(interval);

  interval_print(interval);
  mapping_print(mapping);

  interval_free(interval);
  mapping_free(mapping);
  return 0;
}
