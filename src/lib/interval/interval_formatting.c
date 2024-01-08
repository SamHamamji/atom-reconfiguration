#include <stdio.h>
#include <stdlib.h>

#include "interval.h"

void interval_print(const struct Interval *interval) {
  if (interval->length == 0) {
    printf("Empty interval");
  }

  for (int i = 0; i < interval->length; i++) {
    printf("%s ", point_to_string(interval->array[i]));
  }
  printf("\n");
}
