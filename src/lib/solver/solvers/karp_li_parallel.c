#include <assert.h>
#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../common/height_array.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

struct ThreadInputContext {
  const int *height_array;
  const struct Interval *interval;
  int *output;
};

struct ThreadInput {
  struct ThreadInputContext context;
  struct {
    int min_height;
    int max_height;
  } height_range;
};

static void *get_exclusion_for_height_range_thread(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;
  const int output_length =
      input->height_range.max_height - input->height_range.min_height;
  int *excluded_indexes = malloc(output_length * sizeof(int));

  int i = input->context.interval->length - 1;

  for (int height = input->height_range.max_height - 1;
       height >= input->height_range.min_height; height--) {
    int max_profit_index = INT_MAX;
    int profit = INT_MAX;

    for (; i >= 0; i--) {
      if (input->context.height_array[i] == height) {
        if (input->context.interval->array[i].is_source) {
          profit -= i;
          if (profit > 0) {
            max_profit_index = i;
            profit = 0;
          }
          profit -= i;
        }
        if (input->context.interval->array[i].is_target) {
          profit += 2 * i;
        }
      }
    }

    i = max_profit_index - 1;
    excluded_indexes[height - input->height_range.min_height] =
        max_profit_index;
  }

  memcpy(&input->context.output[input->height_range.min_height - 1],
         excluded_indexes, output_length * sizeof(int));

  free(excluded_indexes);
  pthread_exit(NULL);
}

static bool *get_exclusion_array(const struct Interval *interval,
                                 const int *height_array, int thread_num) {
  const int imbalance = get_imbalance_from_height_array(interval, height_array);
  int *excluded_indexes = malloc(imbalance * sizeof(int));
  pthread_t *thread_array = malloc(thread_num * sizeof(pthread_t));
  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));

  const int heights_per_thread = imbalance / thread_num;
  const int remaining_heights = imbalance % thread_num;
  const struct ThreadInputContext context = {
      .height_array = height_array,
      .interval = interval,
      .output = excluded_indexes,
  };

  for (int i = 0; i < thread_num; i++) {
    thread_inputs[i].height_range.min_height =
        (i == 0 ? 1 : thread_inputs[i - 1].height_range.max_height);
    thread_inputs[i].height_range.max_height =
        thread_inputs[i].height_range.min_height + heights_per_thread +
        (int)(i < remaining_heights);
    thread_inputs[i].context = context;

    pthread_create(&thread_array[i], NULL,
                   get_exclusion_for_height_range_thread,
                   (void *)&thread_inputs[i]);
  }

  bool *exclusion_array = calloc(interval->length, sizeof(bool));
  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_array[i], NULL);
    for (int height = thread_inputs[i].height_range.min_height;
         height < thread_inputs[i].height_range.max_height; height++) {
      exclusion_array[excluded_indexes[height - 1]] = true;
    }
  }

  free(thread_inputs);
  free(thread_array);
  free(excluded_indexes);
  return exclusion_array;
}

struct Mapping *
karp_li_parallel_solver_function(const struct Interval *interval,
                                 const void *params) {
  assert(params != NULL);
  int thread_num = ((KarpLiParallelParams *)params)->thread_num;
  if (interval->length <= 0) {
    return mapping_get_null();
  }

  int *height_array = get_height_array(interval);
  if (get_imbalance_from_height_array(interval, height_array) < 0) {
    free(height_array);
    return mapping_get_null();
  }

  bool *exclusion_array =
      get_exclusion_array(interval, height_array, thread_num);
  free(height_array);

  struct Mapping *mapping = solve_neutral_interval(
      interval, exclusion_array, interval_get_counts(interval).target_num);

  free(exclusion_array);
  return mapping;
}
