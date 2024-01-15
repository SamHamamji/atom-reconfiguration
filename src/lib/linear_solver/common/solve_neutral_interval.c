#include <assert.h>
#include <stdlib.h>

#include "solve_neutral_interval.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array,
                                       int target_num) {
  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  mapping->pairs = malloc(target_num * sizeof(struct Pair));
  mapping->pair_count = target_num;

  solve_neutral_interval_range(
      interval, exclusion_array,
      (struct Range){.start = 0, .exclusive_end = interval->length}, 0, 0,
      mapping);

  return mapping;
}

void solve_neutral_interval_range(const struct Interval *interval,
                                  const bool *exclusion_array,
                                  struct Range range, int first_source,
                                  int first_target, struct Mapping *mapping) {
  assert(0 <= range.start && range.exclusive_end <= interval->length);
  for (int i = range.start; i < range.exclusive_end; i++) {
    if (interval->array[i].is_source && !exclusion_array[i]) {
      mapping->pairs[first_source].source = i;
      first_source++;
    }
    if (interval->array[i].is_target) {
      mapping->pairs[first_target].target = i;
      first_target++;
    }
  }
}

void solve_neutral_interval_half(const struct Interval *interval,
                                 const bool *exclusion_array,
                                 struct Mapping *mapping, bool solve_source,
                                 bool solve_first_half) {
  if (interval->length == 0) {
    return;
  }

  int counter, start, increment;
  int middle = interval->length / 2 + (int)solve_first_half;
  if (solve_first_half) {
    counter = 0;
    start = 0;
    increment = 1;
  } else {
    counter = mapping->pair_count - 1;
    start = interval->length - 1;
    increment = -1;
  }

  if (solve_source) {
    for (int i = start; i != middle; i += increment) {
      if (interval->array[i].is_source && !exclusion_array[i]) {
        mapping->pairs[counter].source = i;
        counter += increment;
      }
    }
  } else {
    for (int i = start; i != middle; i += increment) {
      if (interval->array[i].is_target) {
        mapping->pairs[counter].target = i;
        counter += increment;
      }
    }
  }
}
