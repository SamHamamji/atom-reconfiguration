#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

struct ThreadInput {
  int height;
  struct Interval interval;
  const int *height_array;
  int *output;
};

static void *get_exclusion_for_height_thread(void *const args) {
  struct ThreadInput *input = (struct ThreadInput *)args;

  int relative_profit = 0;
  int max_profit_index = INT_MAX;

  for (int i = input->interval.size - 1; i >= 0; i--) {
    relative_profit += (input->height_array[i] >= input->height) ? 1 : -1;
    if ((input->height_array[i] == input->height) &&
        (input->interval.array[i] == SOURCE) && (relative_profit > 0)) {
      relative_profit = 0;
      max_profit_index = i;
    }
  }

  *input->output = max_profit_index;
  pthread_exit(NULL);
}

static int *get_exclusion_array(const struct Interval *interval,
                                const int *height_array) {
  const int imbalance = height_array[interval->size - 1];
  int exclusion_per_height[imbalance];
  pthread_t thread_array[imbalance];
  struct ThreadInput inputs[imbalance];

  for (int height = 1; height <= imbalance; height++) {
    inputs[height - 1].height = height;
    inputs[height - 1].interval = *interval;
    inputs[height - 1].height_array = height_array;
    inputs[height - 1].output = &exclusion_per_height[height - 1];

    pthread_create(&thread_array[height - 1], NULL,
                   get_exclusion_for_height_thread,
                   (void *)&inputs[height - 1]);
  }

  int *exclusion_array = calloc(interval->size, sizeof(int));
  for (int height = 1; height <= imbalance; height++) {
    pthread_join(thread_array[height - 1], NULL);
    exclusion_array[exclusion_per_height[height - 1]] = 1;
  }

  return exclusion_array;
}

static struct Mapping *
parallel_solver_function(const struct Interval *const interval) {
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

const struct Solver parallel_solver = {
    .solve = parallel_solver_function,
    .name = "Parallel solver",
};
