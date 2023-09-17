#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

#include "solvers.h"

struct Mapping *solve_neutral_interval(struct Interval *interval,
                                       int *exclusion_array) {
  unsigned int target_num = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i] == TARGET) {
      target_num++;
    }
  }
  struct Pair *pairs = (struct Pair *)malloc(target_num * sizeof(struct Pair));

  unsigned int source_counter = 0;
  unsigned int target_counter = 0;
  for (unsigned int i = 0; i < interval->size; i++) {
    if (interval->array[i] == SOURCE && !exclusion_array[i]) {
      pairs[source_counter].source = i;
      source_counter++;
    } else if (interval->array[i] == TARGET) {
      pairs[target_counter].target = i;
      target_counter++;
    }
  }

  struct Mapping *mapping = malloc(sizeof(struct Mapping));
  mapping->pairs = pairs;
  mapping->pair_count = target_num;

  return mapping;
}

int *get_height_array(struct Interval *interval) {
  int *height_array = malloc(interval->size * sizeof(int));
  height_array[0] = 0;

  for (unsigned int i = 1; i < interval->size; i++) {
    height_array[i] = height_array[i - 1];
    if (interval->array[i] == SOURCE) {
      height_array[i] += 1;
    } else if (interval->array[i] == TARGET) {
      height_array[i] -= 1;
    }
  }
  return height_array;
}

int *get_exclusion_array(struct Interval *interval, int *height_array) {
  int imbalance = height_array[interval->size - 1];

  struct {
    int index;
    int value;
  } max_profit_per_height[imbalance];

  for (int height = imbalance; height >= 1; height--) {
    int profit = 0;
    max_profit_per_height[height - 1].index = -1;
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

struct Mapping *iterative_solver(struct Interval *interval) {
  int *height_array = get_height_array(interval);
  int *exclusion_array = get_exclusion_array(interval, height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(height_array);
  free(exclusion_array);
  return mapping;
}
