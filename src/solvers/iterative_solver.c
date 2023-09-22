#include <limits.h>
#include <stdlib.h>

#include "solvers.h"
#include "utils.h"

int *get_exclusion_array_iterative(const struct Interval *interval,
                                   const int *height_array) {
  int imbalance = height_array[interval->size - 1];

  struct {
    int index;
    int value;
  } max_profit_per_height[imbalance];

  for (int height = imbalance; height >= 1; height--) {
    int profit = 0;
    max_profit_per_height[height - 1].index = INT_MAX;
    max_profit_per_height[height - 1].value = INT_MIN;

    for (int i = (height != imbalance) ? max_profit_per_height[height].index - 1
                                       : interval->size - 1;
         i >= 0; i--) {
      profit += (height_array[i] >= height) ? 1 : -1;
      if ((height_array[i] == height) && (interval->array[i] == SOURCE) &&
          (profit >= max_profit_per_height[height - 1].value)) {
        max_profit_per_height[height - 1].value = profit;
        max_profit_per_height[height - 1].index = i;
      }
    }
  }

  int *exclusion_array = calloc(interval->size, sizeof(int));
  for (int height = 1; height <= imbalance; height++) {
    exclusion_array[max_profit_per_height[height - 1].index] = 1;
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
