#include <stdio.h>
#include <stdlib.h>

#include "interval/interval.h"
#include "main.h"

int main() {
  const Point points[] = {EMPTY,  TARGET, SOURCE, EMPTY,  TARGET, SOURCE,
                          SOURCE, SOURCE, EMPTY,  TARGET, SOURCE, EMPTY};
  const int size = sizeof(points) / sizeof(points[0]);

  struct Interval *interval = new_interval(points, size);

  char *output = interval_to_string(interval);
  printf("%s\n", output);
  free(output);

  interval_free(interval);

  return 0;
}
