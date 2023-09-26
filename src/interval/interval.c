#include <stdlib.h>

#include "interval.h"

static struct Interval *generate_random_interval(const int size) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->size = size;
  interval->array = calloc(size, sizeof(Point));
  for (int i = 0; i < size; i++) {
    interval->array[i] = rand() % NUM_POINT_TYPES;
  }
  return interval;
}

static struct Interval *new_interval(const Point *const points, int size) {
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

const struct IntervalFactory interval_factory = {
    .generate_random_interval = generate_random_interval,
    .new_interval = new_interval,
};

void interval_free(struct Interval *self) {
  free(self->array);
  free(self);
}
