#include <limits.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

static bool *get_exclusion_array(const struct Interval *interval,
                                 const int *height_array) {
  int imbalance = height_array[interval->size - 1] -
                  (int)interval->array[interval->size - 1].is_target;
  int max_profit_index_per_height[imbalance];
  int i = interval->size - 1;

  for (int height = imbalance; height >= 1; height--) {
    max_profit_index_per_height[height - 1] = INT_MAX;
    int profit = INT_MAX;

    for (; i >= 0; i--) {
      if (height_array[i] == height) {
        if (interval->array[i].is_source) {
          profit -= i;
          if (profit > 0) {
            max_profit_index_per_height[height - 1] = i;
            profit = 0;
          }
          profit -= i;
        }
        if (interval->array[i].is_target) {
          profit += 2 * i;
        }
      }
    }

    i = max_profit_index_per_height[height - 1] - 1;
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

  int *height_array = get_height_array(interval);
  if (height_array[interval->size - 1] < 0) {
    free(height_array);
    return mapping_get_null();
  }

  for (int i = 0; i < interval->size; i++) {
    if (interval->array[i].is_target) {
      height_array[i] += 1;
    }
  }

  bool *exclusion_array = get_exclusion_array(interval, height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(height_array);
  free(exclusion_array);
  return mapping;
}

const struct Solver karp_li_solver_optimized = {
    .solve = solver_function,
    .name = "Karp-Li solver optimized",
};
