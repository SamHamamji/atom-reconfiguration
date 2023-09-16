#include <stdio.h>
#include <stdlib.h>

#include "interval/interval.h"
#include "main.h"
#include "mapping/mapping.h"

int main() {
  const Point points[] = {EMPTY,  EMPTY,  SOURCE, TARGET, TARGET, SOURCE,
                          SOURCE, SOURCE, EMPTY,  TARGET, TARGET, EMPTY};
  const int size = sizeof(points) / sizeof(points[0]);

  struct Interval *interval = new_interval(points, size);
  interval_print(interval);

  struct Mapping *mapping = solve_interval(interval);

  mapping_print(mapping);

  interval_free(interval);
  mapping_free(mapping);

  return 0;
}
