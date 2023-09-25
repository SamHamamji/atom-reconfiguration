#include <limits.h>
#include <stdlib.h>

#include "common.h"
#include "solvers.h"

static int *get_exclusion_array_karp_li(const struct Interval *interval,
                                        const int *height_array) {
  int imbalance = height_array[interval->size - 1];

  int max_profit_index_per_height[imbalance];

  for (int height = imbalance; height >= 1; height--) {
    max_profit_index_per_height[height - 1] = INT_MAX;

    int profit = interval->size - 1;
    for (int i = interval->size - 1; i >= 0; i--) {
      if (interval->array[i] == TARGET && height_array[i] == height - 1) {
        profit += 2 * i;
      } else if (interval->array[i] == SOURCE && height_array[i] == height) {
        profit -= i;
        if (profit >= 0) {
          max_profit_index_per_height[height - 1] = i;
          profit = 0;
        }
        profit -= i;
      }
    }
  }

  int *exclusion_array = calloc(interval->size, sizeof(int));
  for (int height = 1; height <= imbalance; height++) {
    exclusion_array[max_profit_index_per_height[height - 1]] = 1;
  }

  return exclusion_array;
}

struct Mapping *karp_li_solver(const struct Interval *interval) {
  struct Mapping *mapping;
  int *height_array = get_height_array(interval);

  if (height_array[interval->size - 1] < 0) {
    mapping = malloc(sizeof(struct Mapping));
    mapping->pair_count = 0;
    mapping->pairs = NULL;
  } else {
    int *exclusion_array = get_exclusion_array_karp_li(interval, height_array);
    mapping = solve_neutral_interval(interval, exclusion_array);

    free(exclusion_array);
  }

  free(height_array);
  return mapping;
}
