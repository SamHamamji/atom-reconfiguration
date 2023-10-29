#include <limits.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

static bool *get_exclusion_array(const struct Interval *interval,
                                 const int *iterative_height_array) {
  const int imbalance = iterative_height_array[interval->size - 1];
  int max_profit_index_per_height[imbalance];

  for (int height = imbalance; height >= 1; height--) {
    int relative_profit = 0;
    max_profit_index_per_height[height - 1] = INT_MAX;

    for (int i = interval->size - 1; i >= 0; i--) {
      relative_profit += (iterative_height_array[i] >= height) ? 1 : -1;
      if ((iterative_height_array[i] == height) &&
          (interval->array[i].is_source) && (relative_profit > 0)) {
        relative_profit = 0;
        max_profit_index_per_height[height - 1] = i;
      }
    }
  }

  bool *exclusion_array = calloc(interval->size, sizeof(bool));
  for (int height = 1; height <= imbalance; height++) {
    exclusion_array[max_profit_index_per_height[height - 1]] = true;
  }

  return exclusion_array;
}

static struct Mapping *solver_function(const struct Interval *const interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  int *iterative_height_array = get_height_array(interval);
  if (get_imbalance(interval, iterative_height_array) < 0) {
    free(iterative_height_array);
    return mapping_get_null();
  }

  for (int i = 0; i < interval->size; i++) {
    if (interval->array[i].is_target) {
      iterative_height_array[i] -= 1;
    }
  }

  bool *exclusion_array = get_exclusion_array(interval, iterative_height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(iterative_height_array);
  free(exclusion_array);
  return mapping;
}

const struct Solver iterative_solver_naive = {
    .solve = solver_function,
    .name = "Naive Iterative solver",
};
