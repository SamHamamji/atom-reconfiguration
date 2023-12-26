#include <assert.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

static pthread_barrier_t barrier;
static pthread_mutex_t mutex;

struct ThreadInputContext {
  const struct Interval *interval;
  struct AlternatingChains *chains;
  struct Mapping *mapping;
  bool *exclusion_array;
  struct IntervalCounts *global_counts;
  struct IntervalCounts *thread_counts_array;
  struct ThreadIndexes {
    int first_source_index;
    int first_target_index;
  } *first_indexes;
  int thread_num;
};

struct ThreadInput {
  struct ThreadInputContext context;
  int thread_index;
};

struct ThreadsConfig {
  pthread_t *ids;
  struct ThreadInput *inputs;
};

static void thread_config_free(struct ThreadsConfig config) {
  free(config.ids);
  free(config.inputs);
}

static void *solve_interval_range(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;

  struct Range interval_range =
      get_range(input->thread_index, input->context.thread_num,
                input->context.interval->length);

  input->context.thread_counts_array[input->thread_index] =
      interval_get_counts_from_range(input->context.interval, interval_range);

  pthread_mutex_lock(&mutex);
  input->context.global_counts->source_num +=
      input->context.thread_counts_array[input->thread_index].source_num;
  input->context.global_counts->target_num +=
      input->context.thread_counts_array[input->thread_index].target_num;
  pthread_mutex_unlock(&mutex);

  pthread_barrier_wait(&barrier);

  const int imbalance =
      get_imbalance_from_counts(*input->context.global_counts);
  if (imbalance < 0) {
    if (input->thread_index == 0) {
      input->context.mapping->pairs = malloc(0);
      input->context.mapping->pair_count = 0;

      input->context.chains->chain_start_indexes = malloc(0);
      input->context.chains->right_partners = malloc(0);
    }

    pthread_exit(NULL);
  }

  if (input->thread_index == 0) {
    input->context.chains->chain_start_indexes =
        malloc(imbalance * sizeof(int));
    input->context.chains->right_partners =
        malloc(input->context.interval->length * sizeof(int));

    input->context.mapping->pairs =
        malloc(input->context.global_counts->target_num * sizeof(struct Pair));
    input->context.mapping->pair_count =
        input->context.global_counts->target_num;

    input->context.first_indexes[0].first_source_index = 0;
    input->context.first_indexes[0].first_target_index = 0;
    for (int i = 1; i < input->context.thread_num; i++) {
      input->context.first_indexes[i].first_source_index =
          input->context.first_indexes[i - 1].first_source_index +
          input->context.thread_counts_array[i - 1].source_num;
      input->context.first_indexes[i].first_target_index =
          input->context.first_indexes[i - 1].first_target_index +
          input->context.thread_counts_array[i - 1].target_num;
    }
  }

  pthread_barrier_wait(&barrier);
  // Compute chains

  const struct Range chain_range =
      get_range(input->thread_index, input->context.thread_num, imbalance);
  alternating_chains_compute_range(input->context.interval,
                                   input->context.chains, chain_range);

  // Solve every chain
  const int chain_range_length = chain_range.exclusive_end - chain_range.start;
  int *excluded_indexes = alternating_chains_get_exclusion_from_range(
      input->context.chains, chain_range, input->context.interval->length);

  for (int i = 0; i < chain_range_length; i++) {
    input->context.exclusion_array[excluded_indexes[i]] = true;
  }

  pthread_barrier_wait(&barrier);

  for (int i = 0; i < chain_range_length; i++) {
    int excluded_source_range =
        get_range_index(excluded_indexes[i], input->context.thread_num,
                        input->context.interval->length);

    pthread_mutex_lock(&mutex);
    for (int j = excluded_source_range + 1; j < input->context.thread_num;
         j++) {
      input->context.first_indexes[j].first_source_index--;
    }
    pthread_mutex_unlock(&mutex);
  }

  pthread_barrier_wait(&barrier);

  free(excluded_indexes);

  solve_neutral_interval_range(
      input->context.interval, input->context.exclusion_array, interval_range,
      input->context.first_indexes[input->thread_index].first_source_index,
      input->context.first_indexes[input->thread_index].first_target_index,
      input->context.mapping);

  pthread_exit(NULL);
}

struct Mapping *
aggarwal_parallel_solver_function(const struct Interval *interval,
                                  const void *params) {
  assert(params != NULL);
  int thread_num = ((AggarwalParallelParams *)params)->thread_num;

  if (interval->length <= 0) {
    return mapping_get_null();
  }
  pthread_barrier_init(&barrier, NULL, thread_num);
  pthread_mutex_init(&mutex, NULL);

  const struct ThreadInputContext context = {
      .interval = interval,
      .mapping = malloc(sizeof(struct Mapping)),
      .chains = malloc(sizeof(struct AlternatingChains)),
      .exclusion_array = malloc(interval->length * sizeof(bool)),
      .global_counts = malloc(sizeof(struct IntervalCounts)),
      .thread_counts_array = malloc(thread_num * sizeof(struct IntervalCounts)),
      .first_indexes = malloc(thread_num * sizeof(struct ThreadIndexes)),
      .thread_num = thread_num,
  };

  context.global_counts->source_num = 0;
  context.global_counts->target_num = 0;
  memset(context.exclusion_array, 0, context.interval->length);

  struct ThreadsConfig thread_config = {
      .ids = malloc(thread_num * sizeof(pthread_t)),
      .inputs = malloc(thread_num * sizeof(struct ThreadInput)),
  };
  for (int i = 0; i < thread_num; i++) {
    thread_config.inputs[i].context = context;
    thread_config.inputs[i].thread_index = i;
    pthread_create(&thread_config.ids[i], NULL, solve_interval_range,
                   &thread_config.inputs[i]);
  }

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_config.ids[i], NULL);
  }

  thread_config_free(thread_config);
  alternating_chains_free(context.chains);

  free(context.thread_counts_array);
  free(context.exclusion_array);
  free(context.global_counts);
  free(context.first_indexes);

  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&mutex);

  return context.mapping;
}
