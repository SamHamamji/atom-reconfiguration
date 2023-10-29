#include <stdlib.h>

#include "common.h"

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

int *get_height_array(const struct Interval *interval) {
  if (interval->size <= 0) {
    return NULL;
  }

  int *height_array = malloc(interval->size * sizeof(int));
  height_array[0] = (int)interval->array[0].is_source;

  for (unsigned int i = 1; i < interval->size; i++) {
    height_array[i] = height_array[i - 1] + (int)interval->array[i].is_source -
                      (int)interval->array[i - 1].is_target;
  }

  return height_array;
}

int inline get_imbalance(const struct Interval *interval,
                         const int *height_array) {
  return height_array[interval->size - 1] -
         (int)interval->array[interval->size - 1].is_target;
}
