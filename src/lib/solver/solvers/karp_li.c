#include <limits.h>
#include <stdlib.h>

#include "../common/height_array.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

static bool *get_exclusion_array(const struct Interval *interval,
                                 const int *height_array) {
  const int imbalance = get_imbalance_from_height_array(interval, height_array);
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

struct Mapping *karp_li_solver_function(const struct Interval *interval,
                                        const void *params) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  int *height_array = get_height_array(interval);
  if (get_imbalance_from_height_array(interval, height_array) < 0) {
    free(height_array);
    return mapping_get_null();
  }

  bool *exclusion_array = get_exclusion_array(interval, height_array);
  free(height_array);

  struct Mapping *mapping = solve_neutral_interval(
      interval, exclusion_array, interval_get_counts(interval).target_num);
  free(exclusion_array);
  return mapping;
}
