#include <assert.h>
#include <stdlib.h>

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

static void interval_shuffle(struct Interval *interval) {
  for (int i = 0; i < interval->length; i++) {
    struct Point current = interval->array[i];
    int source_index = rand() % interval->length;
    int target_index = rand() % interval->length;
    interval->array[i].is_source = interval->array[source_index].is_source;
    interval->array[i].is_target = interval->array[target_index].is_target;
    interval->array[source_index].is_source = current.is_source;
    interval->array[target_index].is_target = current.is_target;
  }
}

static struct Interval *new_interval(int length) {
  struct Interval *interval = malloc(sizeof(struct Interval));
  interval->array = malloc(length * sizeof(struct Point));
  interval->length = length;

  return interval;
}

static struct Interval *generate_interval(int length) {
  struct Interval *interval = new_interval(length);

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
  struct Interval *interval = new_interval(length);

  int i = 0;
  for (; i < abs(imbalance); i++) {
    interval->array[i] =
        (imbalance > 0) ? (struct Point)SOURCE : (struct Point)TARGET;
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
