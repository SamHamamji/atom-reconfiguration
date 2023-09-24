#include <limits.h>
#include <stdlib.h>

#include "common.h"
#include "solvers.h"

static int *get_exclusion_array_iterative(const struct Interval *interval,
                                          const int *height_array) {
  const int imbalance = height_array[interval->size - 1];

  int max_profit_index_per_height[imbalance];

  for (int height = imbalance; height >= 1; height--) {
    int relative_profit = 0;
    max_profit_index_per_height[height - 1] = INT_MAX;

    for (int i = (height != imbalance) ? max_profit_index_per_height[height] - 1
                                       : interval->size - 1;
         i >= 0; i--) {
      relative_profit += (height_array[i] >= height) ? 1 : -1;
      if ((height_array[i] == height) && (interval->array[i] == SOURCE) &&
          (relative_profit >= 0)) {
        relative_profit = 0;
        max_profit_index_per_height[height - 1] = i;
      }
    }
  }

  int *exclusion_array = calloc(interval->size, sizeof(int));
  for (int height = 1; height <= imbalance; height++) {
    exclusion_array[max_profit_index_per_height[height - 1]] = 1;
  }

  return exclusion_array;
}

struct Mapping *iterative_solver(const struct Interval *interval) {
  int *height_array = get_height_array(interval);
  int *exclusion_array = get_exclusion_array_iterative(interval, height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(height_array);
  free(exclusion_array);
  return mapping;
}
