#include <stdio.h>
#include <stdlib.h>

#include "interval/interval.h"
#include "main.h"
#include "mapping/mapping.h"
#include "solvers/solvers.h"

int main() {
  const Point points[] = {EMPTY,  EMPTY,  SOURCE, SOURCE, TARGET, TARGET,
                          TARGET, SOURCE, TARGET, SOURCE, SOURCE, EMPTY,
                          TARGET, SOURCE, SOURCE, SOURCE, SOURCE, EMPTY,
                          TARGET, TARGET, EMPTY};

  const int size = sizeof(points) / sizeof(points[0]);

  struct Interval *interval = new_interval(points, size);
  interval_print(interval);

  struct Mapping *mapping = iterative_solver(interval);
  mapping_print(mapping);

  interval_free(interval);
  mapping_free(mapping);

  return 0;
}
