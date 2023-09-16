#include <stdio.h>
#include <stdlib.h>

#include "mapping.h"

struct Mapping *solve_neutral_interval(struct Interval *interval,
                                       int target_num) {
  struct Pair *pairs = (struct Pair *)malloc(target_num * sizeof(struct Pair));

  unsigned int source_counter = 0;
  unsigned int target_counter = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i] == SOURCE) {
      pairs[source_counter].source = i;
      source_counter++;
    } else if (interval->array[i] == TARGET) {
      pairs[target_counter].target = i;
      target_counter++;
    }
  }

  if (source_counter != target_counter || target_counter != target_num) {
    fprintf(stderr, "ERROR: interval is not neutral, |S| = %u, |T| = %u\n",
            source_counter, target_counter);
    exit(EXIT_FAILURE);
  }

  struct Mapping *mapping_ptr = malloc(sizeof(struct Mapping));
  mapping_ptr->pairs = pairs;
  mapping_ptr->pair_count = target_num;

  return mapping_ptr;
}

struct Mapping *solve_interval(struct Interval *interval) {
  unsigned int source_num = 0;
  unsigned int target_num = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i] == SOURCE) {
      source_num++;
    } else if (interval->array[i] == TARGET) {
      target_num++;
    }
  }
  if (source_num == target_num) {
    return solve_neutral_interval(interval, target_num);
  } else {
    fprintf(stderr, "Interval is not neutral, implementation coming soon...\n");
    exit(EXIT_FAILURE);
  }
}

void mapping_free(struct Mapping *mapping) {
  free(mapping->pairs);
  free(mapping);
}
