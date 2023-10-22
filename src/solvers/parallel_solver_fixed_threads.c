#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

#define MAX_THREADS_NUMBER 10

struct ThreadInput {
  struct {
    int min_height;
    int max_height;
  } height_range;
  const int *height_array;
  struct Interval interval;
  int *output;
};

static void *get_exclusion_for_height_thread(void *const args) {
  const struct ThreadInput *const input = (struct ThreadInput *)args;

  for (int height = input->height_range.min_height;
       height < input->height_range.max_height; height++) {
    int relative_profit = 0;
    int max_profit_index = INT_MAX;

    for (int i = input->interval.size - 1; i >= 0; i--) {
      relative_profit += (input->height_array[i] >= height) ? 1 : -1;
      if ((input->height_array[i] == height) &&
          (input->interval.array[i] == SOURCE) && (relative_profit > 0)) {
        relative_profit = 0;
        max_profit_index = i;
      }
    }

    input->output[height - 1] = max_profit_index;
  }

  pthread_exit(NULL);
}

static int *get_exclusion_array_parallel(const struct Interval *interval,
                                         const int *height_array) {
  const int imbalance = height_array[interval->size - 1];
  int *threads_output = malloc(imbalance * sizeof(int));
  pthread_t thread_array[MAX_THREADS_NUMBER];
  struct ThreadInput inputs[MAX_THREADS_NUMBER];

  const int heights_per_thread = imbalance / MAX_THREADS_NUMBER;
  const int remaining_heights = imbalance % MAX_THREADS_NUMBER;

  for (int i = 0; i < MAX_THREADS_NUMBER; i++) {
    inputs[i].height_range.min_height =
        (i == 0 ? 1 : inputs[i - 1].height_range.max_height);
    inputs[i].height_range.max_height = inputs[i].height_range.min_height +
                                        heights_per_thread +
                                        (i < remaining_heights ? 1 : 0);
    inputs[i].interval = *interval;
    inputs[i].height_array = height_array;
    inputs[i].output = threads_output;

    pthread_create(&thread_array[i], NULL, get_exclusion_for_height_thread,
                   (void *)&inputs[i]);
  }

  int *exclusion_array = calloc(interval->size, sizeof(int));
  for (int i = 0; i < MAX_THREADS_NUMBER; i++) {
    pthread_join(thread_array[i], NULL);
    for (int height = inputs[i].height_range.min_height;
         height < inputs[i].height_range.max_height; height++) {
      exclusion_array[threads_output[height - 1]] = 1;
    }
  }

  free(threads_output);
  return exclusion_array;
}

static struct Mapping *
parallel_solver_test_function(const struct Interval *const interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  int *height_array = get_height_array(interval);
  if (height_array[interval->size - 1] < 0) {
    free(height_array);
    return mapping_get_null();
  }

  int *exclusion_array = get_exclusion_array_parallel(interval, height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(height_array);
  free(exclusion_array);
  return mapping;
}

const struct Solver parallel_solver_fixed_threads = {
    .solve = parallel_solver_test_function,
    .name = "Parallel solver with fixed threads",
};
