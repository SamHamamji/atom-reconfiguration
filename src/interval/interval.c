#include <stdio.h>
#include <stdlib.h>

#include "interval.h"

static struct Interval *generate_randomized_interval(const int size) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->size = size;
  interval->array = calloc(size, sizeof(Point));
  for (int i = 0; i < size; i++) {
    interval->array[i] = rand() % NUM_POINT_TYPES;
  }
  return interval;
}

static struct Interval *new_interval(const Point *const points,
                                     const int size) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->array = malloc(size * sizeof(Point));
  interval->size = size;

  for (int i = 0; i < size; i++) {
    interval->array[i] = points[i];
  }

  return interval;
}

static void interval_shuffle(struct Interval *const interval) {
  for (int i = 0; i < interval->size; i++) {
    int j = rand() % interval->size;
    int temp = interval->array[j];
    interval->array[j] = interval->array[i];
    interval->array[i] = temp;
  }
}

static struct Interval *generate_interval(const int size, const int target_num,
                                          const int source_num) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->size = size;
  interval->array = malloc(interval->size * sizeof(Point));

  if (size < target_num + source_num) {
    printf("Invalid interval parameters (size %d, targets %d, sources %d)\n",
           size, target_num, source_num);
    return NULL;
  }

  int i = 0;
  for (; i < target_num; i++) {
    interval->array[i] = TARGET;
  }
  for (; i < target_num + source_num; i++) {
    interval->array[i] = SOURCE;
  }
  for (; i < interval->size; i++) {
    interval->array[i] = EMPTY;
  }
  interval_shuffle(interval);
  return interval;
}

const struct IntervalFactory interval_factory = {
    .generate_randomized_interval = generate_randomized_interval,
    .generate_interval = generate_interval,
    .new_interval = new_interval,
};

void interval_free(struct Interval *self) {
  free(self->array);
  free(self);
}
