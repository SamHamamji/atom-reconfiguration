#include <limits.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

static int *get_exclusion_array(const struct Interval *interval,
                                const int *height_array) {
  int imbalance = height_array[interval->size - 1];

  int max_profit_index_per_height[imbalance];

  for (int height = imbalance; height >= 1; height--) {
    max_profit_index_per_height[height - 1] = INT_MAX;
    int profit = INT_MAX;

    for (int i = interval->size - 1; i >= 0; i--) {
      if (interval->array[i] == TARGET && height_array[i] == height - 1) {
        profit += 2 * i;
      } else if (interval->array[i] == SOURCE && height_array[i] == height) {
        profit -= i;
        if (profit > 0) {
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

static struct Mapping *solver_function(const struct Interval *const interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  int *height_array = get_height_array(interval);
  if (height_array[interval->size - 1] < 0) {
    free(height_array);
    return mapping_get_null();
  }

  int *exclusion_array = get_exclusion_array(interval, height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(height_array);
  free(exclusion_array);
  return mapping;
}

const struct Solver karp_li_solver = {
    .solve = solver_function,
    .name = "Karp-Li solver",
};
