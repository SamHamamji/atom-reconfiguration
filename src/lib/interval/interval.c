#include <stdlib.h>
#include <string.h>

#include "interval.h"

static struct Interval *new_interval(const struct Point *points, int length) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->array = malloc(length * sizeof(struct Point));
  interval->length = length;

  memcpy(interval->array, points, length * sizeof(struct Point));

  return interval;
}

struct IntervalCounts
interval_get_counts_from_slice(const struct Interval *interval, int start,
                               int exclusive_end) {
  struct IntervalCounts counts = {0, 0};
  for (int i = start; i < exclusive_end; i++) {
    counts.source_num += interval->array[i].is_source;
    counts.target_num += interval->array[i].is_target;
  }
  return counts;
}

inline struct IntervalCounts
interval_get_counts(const struct Interval *interval) {
  return interval_get_counts_from_slice(interval, 0, interval->length);
}

inline int get_imbalance_from_counts(struct IntervalCounts counts) {
  return counts.source_num - counts.target_num;
}

static void interval_swap_source(struct Interval *interval, int a, int b) {
  bool temp = interval->array[a].is_source;
  interval->array[a].is_source = interval->array[b].is_source;
  interval->array[b].is_source = temp;
}

static void interval_swap_target(struct Interval *interval, int a, int b) {
  bool temp = interval->array[a].is_target;
  interval->array[a].is_target = interval->array[b].is_target;
  interval->array[b].is_target = temp;
}

static void interval_shuffle(struct Interval *interval) {
  for (int i = 0; i < interval->length; i++) {
    int source_index = random() % interval->length;
    int target_index = random() % interval->length;
    interval_swap_source(interval, i, source_index);
    interval_swap_target(interval, i, target_index);
  }
}

static struct Interval *generate_randomized_interval(int length) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->length = length;
  interval->array = calloc(length, sizeof(struct Point));
  for (int i = 0; i < length; i++) {
    interval->array[i] = (struct Point){
        .is_source = (bool)(random() % 2),
        .is_target = (bool)(random() % 2),
    };
  }
  return interval;
}

static struct Interval *generate_interval_by_imbalance(int length,
                                                       int imbalance) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->length = length;
  interval->array = malloc(interval->length * sizeof(struct Point));

  int i = 0;
  for (; i < imbalance; i++) {
    interval->array[i] = (struct Point)SOURCE;
  }

  while (i < length) {
    switch (random() % 3) {
    case 0:
      if (i == length - 1) {
        interval->array[i] = (struct Point)EMPTY;
      } else {
        interval->array[i] = (struct Point)SOURCE;
        interval->array[i + 1] = (struct Point)TARGET;
        i++;
      }
      break;
    case 1:
      interval->array[i] = (struct Point)EMPTY;
      break;
    case 2:
      interval->array[i] = (struct Point)BOTH;
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
