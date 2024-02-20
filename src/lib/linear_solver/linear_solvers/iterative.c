#include <assert.h>
#include <limits.h>
#include <stdlib.h>

#include "../common/height_array.h"
#include "../common/solve_neutral_interval.h"
#include "../linear_solver.h"

static bool *get_exclusion_array(const struct Interval *interval,
                                 const int *iterative_height_array) {
  const int imbalance =
      (interval->length > 0) ? iterative_height_array[interval->length - 1] : 0;
  int *max_profit_index_per_height = malloc(imbalance * sizeof(int));
  int i = interval->length - 1;

  for (int height = imbalance; height > 0; height--) {
    int relative_profit = 0;
    max_profit_index_per_height[height - 1] = INT_MAX;

    for (; i >= 0; i--) {
      relative_profit += (iterative_height_array[i] >= height) ? 1 : -1;
      if ((iterative_height_array[i] == height) &&
          (interval->array[i].is_source) && (relative_profit > 0)) {
        relative_profit = 0;
        max_profit_index_per_height[height - 1] = i;
      }
    }

    i = max_profit_index_per_height[height - 1] - 1;
  }

  bool *exclusion_array = calloc(interval->length, sizeof(bool));
  for (int height = 1; height <= imbalance; height++) {
    exclusion_array[max_profit_index_per_height[height - 1]] = true;
  }

  free(max_profit_index_per_height);

  return exclusion_array;
}

struct Mapping *linear_solve_iterative(const struct Interval *const interval,
                                       const void *params) {
  (void)params; // to avoid unused parameter warning
  assert(params == NULL);

  int *iterative_height_array = get_height_array(interval);
  if (get_imbalance_from_height_array(interval, iterative_height_array) < 0) {
    free(iterative_height_array);
    return NULL;
  }

  for (int i = 0; i < interval->length; i++) {
    if (interval->array[i].is_target) {
      iterative_height_array[i] -= 1;
    }
  }

  bool *exclusion_array = get_exclusion_array(interval, iterative_height_array);
  free(iterative_height_array);

  struct Mapping *mapping = solve_neutral_interval(
      interval, exclusion_array, interval_get_counts(interval).target_num);
  free(exclusion_array);
  return mapping;
}
