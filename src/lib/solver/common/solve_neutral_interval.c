#include <stdlib.h>

#include "solve_neutral_interval.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array,
                                       int target_num) {
  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  mapping->pairs = malloc(target_num * sizeof(struct Pair));
  mapping->pair_count = target_num;

  unsigned int source_counter = 0;
  unsigned int target_counter = 0;
  for (int i = 0; i < interval->size; i++) {
    if (interval->array[i].is_source && !exclusion_array[i]) {
      mapping->pairs[source_counter].source = i;
      source_counter++;
    }
    if (interval->array[i].is_target) {
      mapping->pairs[target_counter].target = i;
      target_counter++;
    }
  }

  return mapping;
}

void solve_neutral_interval_slice(const struct Interval *interval,
                                  const bool *exclusion_array,
                                  struct Mapping *mapping, bool solve_source,
                                  bool solve_first_half) {
  int counter, start, increment;
  int middle = interval->size / 2 + (int)solve_first_half;
  if (solve_first_half) {
    counter = 0;
    start = 0;
    increment = 1;
  } else {
    counter = mapping->pair_count - 1;
    start = interval->size - 1;
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