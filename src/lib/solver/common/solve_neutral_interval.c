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
