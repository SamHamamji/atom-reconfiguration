#include <limits.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

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

static struct Mapping *
iterative_solver_function(const struct Interval *const interval) {
  struct Mapping *mapping;
  int *height_array = get_height_array(interval);

  if (height_array[interval->size - 1] < 0) {
    mapping = malloc(sizeof(struct Mapping));
    mapping->pair_count = 0;
    mapping->pairs = NULL;
  } else {
    int *exclusion_array =
        get_exclusion_array_iterative(interval, height_array);
    mapping = solve_neutral_interval(interval, exclusion_array);

    free(exclusion_array);
  }

  free(height_array);
  return mapping;
}

const struct Solver iterative_solver = {
    .solve = iterative_solver_function,
    .name = "Iterative solver",
};