#include <stdlib.h>

#include "interval.h"

struct Interval *new_interval(const Point *points, int size) {
  struct Interval *interval =
      (struct Interval *)malloc(sizeof(struct Interval));

  Point *array = (Point *)malloc(size * sizeof(Point));

  for (int i = 0; i < size; i++) {
    array[i] = points[i];
  }

  *interval = (struct Interval){
      .array = array,
      .size = size,
  };

  return interval;
}

void interval_free(struct Interval *self) {
  free(self->array);
  free(self);
}
