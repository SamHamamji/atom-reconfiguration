#include "utils.h"

#include <stdlib.h>

struct Mapping *solve_neutral_interval(const struct Interval *interval,
                                       const int *exclusion_array) {
  unsigned int target_num = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i] == TARGET) {
      target_num++;
    }
  }
  struct Pair *pairs = (struct Pair *)malloc(target_num * sizeof(struct Pair));

  unsigned int source_counter = 0;
  unsigned int target_counter = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i] == SOURCE && !exclusion_array[i]) {
      pairs[source_counter].source = i;
      source_counter++;
    } else if (interval->array[i] == TARGET) {
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
  int *height_array = malloc(interval->size * sizeof(int));
  height_array[0] = 0;

  for (unsigned int i = 1; i < interval->size; i++) {
    height_array[i] = height_array[i - 1];
    if (interval->array[i] == SOURCE) {
      height_array[i] += 1;
    } else if (interval->array[i] == TARGET) {
      height_array[i] -= 1;
    }
  }
  return height_array;
}
