#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

static pthread_barrier_t barrier;
static pthread_mutex_t counts_mutex;

struct ThreadInputContext {
  const struct Interval *interval;
  struct AlternatingChains *chains;
  struct Mapping *mapping;
  bool *exclusion_array;
  struct IntervalCounts *counts;
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

inline static int get_min_chain(int thread_index, int thread_num,
                                int imbalance) {
  int remaining_heights = imbalance % thread_num;
  return thread_index * (imbalance / thread_num) +
         (thread_index < remaining_heights ? thread_index : remaining_heights);
}

inline static int get_interval_start_index(int thread_index, int thread_num,
                                           int interval_length) {
  int remaining_length = interval_length % thread_num;
  return thread_index * (interval_length / thread_num) +
         (thread_index < remaining_length ? thread_index : remaining_length);
}

static void *solve_interval_range(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;

  struct IntervalCounts slice_counts = interval_get_counts_from_slice(
      input->context.interval,
      get_interval_start_index(input->thread_index, input->context.thread_num,
                               input->context.interval->length),
      get_interval_start_index(input->thread_index + 1,
                               input->context.thread_num,
                               input->context.interval->length));

  pthread_mutex_lock(&counts_mutex);
  input->context.counts->source_num += slice_counts.source_num;
  input->context.counts->target_num += slice_counts.target_num;
  pthread_mutex_unlock(&counts_mutex);

  pthread_barrier_wait(&barrier);

  const int imbalance = get_imbalance_from_counts(*input->context.counts);
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
        malloc(input->context.counts->target_num * sizeof(struct Pair));
    input->context.mapping->pair_count = input->context.counts->target_num;
  }

  pthread_barrier_wait(&barrier);

  const struct ChainRange chain_range = {
      .min_chain = get_min_chain(input->thread_index, input->context.thread_num,
                                 imbalance),
      .max_chain_exclusive = get_min_chain(
          input->thread_index + 1, input->context.thread_num, imbalance),
  };

  alternating_chains_compute_range(input->context.interval,
                                   input->context.chains, chain_range);

  const int chain_range_length =
      chain_range.max_chain_exclusive - chain_range.min_chain;
  int *excluded_indexes = alternating_chains_get_exclusion_from_range(
      input->context.chains, chain_range, input->context.interval->length);

  for (int i = 0; i < chain_range_length; i++) {
    input->context.exclusion_array[excluded_indexes[i]] = true;
  }

  pthread_barrier_wait(&barrier);

  free(excluded_indexes);

  if (input->thread_index < 4) {
    solve_neutral_interval_slice(
        input->context.interval, input->context.exclusion_array,
        input->context.mapping, (bool)(input->thread_index % 2),
        (bool)(input->thread_index / 2));
  }

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
  pthread_mutex_init(&counts_mutex, NULL);

  const struct ThreadInputContext context = {
      .interval = interval,
      .mapping = malloc(sizeof(struct Mapping)),
      .chains = malloc(sizeof(struct AlternatingChains)),
      .exclusion_array = malloc(interval->length * sizeof(bool)),
      .counts = malloc(sizeof(struct IntervalCounts)),
      .thread_num = thread_num,
  };

  context.counts->source_num = 0;
  context.counts->target_num = 0;
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

  free(context.exclusion_array);
  free(context.counts);

  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&counts_mutex);

  return context.mapping;
}
