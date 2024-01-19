#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "interval.h"

struct Counts interval_get_counts_from_range(const struct Interval *interval,
                                             struct Range range) {
  struct Counts counts = {0, 0};
  for (int i = range.start; i < range.exclusive_end; i++) {
    counts.source_num += interval->array[i].is_source;
    counts.target_num += interval->array[i].is_target;
  }
  return counts;
}

inline struct Counts interval_get_counts(const struct Interval *interval) {
  return interval_get_counts_from_range(interval,
                                        (struct Range){0, interval->length});
}

inline int counts_get_imbalance(struct Counts counts) {
  return counts.source_num - counts.target_num;
}

bool interval_target_region_is_compact(const struct Interval *interval) {
  bool first_target_is_found = false;
  bool last_target_is_found = false;
  for (int i = 0; i < interval->length; i++) {
    if (interval->array[i].is_target) {
      first_target_is_found = true;
      if (last_target_is_found) {
        return false;
      }
    } else {
      if (first_target_is_found) {
        last_target_is_found = true;
      }
    }
  }
  return true;
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
    int source_index = rand() % interval->length;
    int target_index = rand() % interval->length;
    interval_swap_source(interval, i, source_index);
    interval_swap_target(interval, i, target_index);
  }
}

static struct Interval *new_interval(const struct Point *points, int length) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->array = malloc(length * sizeof(struct Point));
  interval->length = length;

  memcpy(interval->array, points, length * sizeof(struct Point));

  return interval;
}

static struct Interval *generate_interval(int length) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->length = length;
  interval->array = calloc(length, sizeof(struct Point));
  for (int i = 0; i < length; i++) {
    interval->array[i] = (struct Point){
        .is_source = (bool)(rand() % 2),
        .is_target = (bool)(rand() % 2),
    };
  }
  return interval;
}

static struct Interval *generate_interval_by_imbalance(int length,
                                                       int imbalance) {
  assert(-length <= imbalance && imbalance <= length);
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->length = length;
  interval->array = malloc(interval->length * sizeof(struct Point));

  int i = 0;
  for (; i < imbalance; i++) {
    interval->array[i] = (struct Point)SOURCE;
  }

  while (i < length) {
    switch (rand() % 3) {
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

static struct Interval *generate_compact_target_region_interval(int length,
                                                                int imbalance) {
  struct Interval *interval = generate_interval_by_imbalance(length, imbalance);

  int target_counter = 0;
  for (int i = 0; i < interval->length; i++) {
    if (interval->array[i].is_target) {
      target_counter++;
    }
    interval->array[i].is_target = false;
  }

  int middle = interval->length / 2;
  for (int i = middle - target_counter / 2; i < middle + target_counter / 2;
       i++) {
    interval->array[i].is_target = true;
  }
  return interval;
}

const struct IntervalFactory interval_factory = {
    .generate = generate_interval,
    .generate_by_imbalance = generate_interval_by_imbalance,
    .generate_compact_target_region = generate_compact_target_region_interval,
    .new_interval = new_interval,
};

void interval_free(struct Interval *self) {
  free(self->array);
  free(self);
}
