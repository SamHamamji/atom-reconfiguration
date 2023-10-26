#include <limits.h>
#include <pthread.h>
#include <stdlib.h>

#include "common.h"
#include "solver.h"

#define CACHE_SIZE 576 * 1024 // 576 KiB
struct ThreadOutput {
  int excluded_index;
  char dummy[CACHE_SIZE - sizeof(int)];
};

struct ThreadInput {
  int height;
  struct Interval interval;
  const int *height_array;
  struct ThreadOutput *output;
};

static void *get_exclusion_for_height_thread(void *const args) {
  const struct ThreadInput *const input = (struct ThreadInput *)args;

  int relative_profit = 0;
  int max_profit_index = INT_MAX;

  for (int i = input->interval.size - 1; i >= 0; i--) {
    relative_profit += (input->height_array[i] >= input->height) ? 1 : -1;
    if ((input->height_array[i] == input->height) &&
        (input->interval.array[i].is_source) && (relative_profit > 0)) {
      relative_profit = 0;
      max_profit_index = i;
    }
  }

  input->output->excluded_index = max_profit_index;
  pthread_exit(NULL);
}

static bool *get_exclusion_array(const struct Interval *interval,
                                 const int *height_array) {
  const int imbalance = height_array[interval->size - 1];
  struct ThreadOutput *output = malloc(imbalance * sizeof(struct ThreadOutput));
  pthread_t thread_array[imbalance];
  struct ThreadInput inputs[imbalance];

  for (int height = 1; height <= imbalance; height++) {
    inputs[height - 1].height = height;
    inputs[height - 1].interval = *interval;
    inputs[height - 1].height_array = height_array;
    inputs[height - 1].output = &output[height - 1];

    pthread_create(&thread_array[height - 1], NULL,
                   get_exclusion_for_height_thread,
                   (void *)&inputs[height - 1]);
  }

  bool *exclusion_array = calloc(interval->size, sizeof(bool));
  for (int height = 1; height <= imbalance; height++) {
    pthread_join(thread_array[height - 1], NULL);
    exclusion_array[output[height - 1].excluded_index] = true;
  }

  free(output);
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

  bool *exclusion_array = get_exclusion_array(interval, height_array);
  struct Mapping *mapping = solve_neutral_interval(interval, exclusion_array);

  free(height_array);
  free(exclusion_array);
  return mapping;
}

const struct Solver parallel_solver_padded = {
    .solve = parallel_solver_test_function,
    .name = "Parallel solver with cache padding",
};
