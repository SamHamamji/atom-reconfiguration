#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "interval.h"

static struct Interval *new_interval(const struct Point *const points,
                                     const int size) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->array = malloc(size * sizeof(struct Point));
  interval->size = size;

  memcpy(interval->array, points, size * sizeof(struct Point));

  return interval;
}

static void interval_shuffle(struct Interval *const interval) {
  for (int i = 0; i < interval->size; i++) {
    int source_index = rand() % interval->size;
    int target_index = rand() % interval->size;
    struct Point temp = interval->array[i];
    interval->array[i].is_source = interval->array[source_index].is_source;
    interval->array[i].is_target = interval->array[target_index].is_target;
    interval->array[source_index].is_source = temp.is_source;
    interval->array[target_index].is_target = temp.is_target;
  }
}

static struct Interval *generate_randomized_interval(const int size) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->size = size;
  interval->array = calloc(size, sizeof(struct Point));
  for (int i = 0; i < size; i++) {
    interval->array[i] = (struct Point){
        .is_source = (bool)rand() % 2,
        .is_target = (bool)rand() % 2,
    };
  }
  return interval;
}

static struct Interval *generate_interval_by_imbalance(const int size,
                                                       const int imbalance) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->size = size;
  interval->array = malloc(interval->size * sizeof(struct Point));

  if (size < imbalance) {
    printf("Invalid interval parameters (size %d, imbalance %d)\n", size,
           imbalance);
    return NULL;
  }

  int i = 0;
  for (; i < imbalance; i++) {
    interval->array[i] = SOURCE;
  }

  while (i < size) {
    switch (rand() % 3) {
    case 0:
      if (i == size - 1) {
        interval->array[i] = EMPTY;
      } else {
        interval->array[i] = SOURCE;
        interval->array[i + 1] = TARGET;
        i++;
      }
      break;
    case 1:
      interval->array[i] = EMPTY;
      break;
    case 2:
      interval->array[i] = BOTH;
      break;
    }
    i++;
  }

  interval_shuffle(interval);
  return interval;
}

const struct IntervalFactory interval_factory = {
    .generate_randomized_interval = generate_randomized_interval,
    .generate_interval_by_imbalance = generate_interval_by_imbalance,
    .new_interval = new_interval,
};

void interval_free(struct Interval *self) {
  free(self->array);
  free(self);
}
