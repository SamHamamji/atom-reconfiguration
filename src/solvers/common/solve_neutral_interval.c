#include <stdlib.h>

#include "solve_neutral_interval.h"

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const bool *exclusion_array) {
  unsigned int target_num = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i].is_target) {
      target_num++;
    }
  }

  struct Pair *pairs = malloc(target_num * sizeof(struct Pair));
  unsigned int source_counter = 0;
  unsigned int target_counter = 0;

  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i].is_source && !exclusion_array[i]) {
      pairs[source_counter].source = i;
      source_counter++;
    }
    if (interval->array[i].is_target) {
      pairs[target_counter].target = i;
      target_counter++;
    }
  }

  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  mapping->pairs = pairs;
  mapping->pair_count = target_num;

  return mapping;
}
