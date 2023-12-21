#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

#define THREAD_NUMBER 8

static pthread_barrier_t barrier;
static pthread_mutex_t mutex;

struct ThreadInputContext {
  const struct Interval *interval;
  struct AlternatingChains *chains;
  struct Mapping *mapping;
  bool *exclusion_array;
  struct IntervalCounts *counts;
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

static void compute_alternating_chain_range(const struct Interval *interval,
                                            struct AlternatingChains *chains,
                                            int min_chain, int max_chain) {
  int *current_chain_node = malloc((max_chain - min_chain) * sizeof(int));
  int current_chain = -1;

  for (int i = 0; i < interval->size; i++) {
    current_chain = current_chain + (int)interval->array[i].is_source -
                    (int)(i != 0 && interval->array[i - 1].is_target);

    if (interval->array[i].is_source == interval->array[i].is_target ||
        current_chain < min_chain || current_chain >= max_chain) {
      continue;
    }

    if (chains->chain_start_indexes[current_chain] == NO_CHAIN_START) {
      chains->chain_start_indexes[current_chain] = i;
    } else {
      chains->right_partners[current_chain_node[current_chain - min_chain]] = i;
    }
    current_chain_node[current_chain - min_chain] = i;
  }

  free(current_chain_node);
}

static int get_min_chain(int thread_index, int imbalance) {
  int remaining_heights = imbalance % THREAD_NUMBER;
  return thread_index * (imbalance / THREAD_NUMBER) +
         (thread_index < remaining_heights ? thread_index : remaining_heights);
}

static int get_interval_start_index(int thread_index, int interval_size) {
  int remaining_size = interval_size % THREAD_NUMBER;
  return thread_index * (interval_size / THREAD_NUMBER) +
         (thread_index < remaining_size ? thread_index : remaining_size);
}

static void *solve_interval_range(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;

  struct IntervalCounts slice_counts = interval_get_counts_from_slice(
      input->context.interval,
      get_interval_start_index(input->thread_index,
                               input->context.interval->size),
      get_interval_start_index(input->thread_index + 1,
                               input->context.interval->size));

  pthread_mutex_lock(&mutex);
  input->context.counts->source_num += slice_counts.source_num;
  input->context.counts->target_num += slice_counts.target_num;
  pthread_mutex_unlock(&mutex);

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
        malloc(input->context.interval->size * sizeof(int));

    for (int i = 0; i < input->context.interval->size; i++) {
      input->context.chains->right_partners[i] = NO_RIGHT_PARTNER;
    }
    for (int i = 0; i < imbalance; i++) {
      input->context.chains->chain_start_indexes[i] = NO_CHAIN_START;
    }
  }

  pthread_barrier_wait(&barrier);

  const int min_chain = get_min_chain(input->thread_index, imbalance);
  const int max_chain = get_min_chain(input->thread_index + 1, imbalance);

  compute_alternating_chain_range(input->context.interval,
                                  input->context.chains, min_chain, max_chain);

  int max_exclusion_index = input->context.interval->size - 1;
  const int chain_range_length = max_chain - min_chain;
  int *excluded_indexes = malloc(chain_range_length * sizeof(int));

  for (int height = max_chain - 1; height >= min_chain; height--) {
    excluded_indexes[height - min_chain] = get_exclusion_from_chain(
        input->context.chains, height, max_exclusion_index);
  }

  for (int i = 0; i < chain_range_length; i++) {
    input->context.exclusion_array[excluded_indexes[i]] = true;
  }

  pthread_barrier_wait(&barrier);

  if (input->thread_index == 0) {
    struct Mapping *mapping = solve_neutral_interval(
        input->context.interval, input->context.exclusion_array,
        input->context.counts->target_num);

    input->context.mapping->pairs = mapping->pairs;
    input->context.mapping->pair_count = mapping->pair_count;

    free(mapping);
  }

  free(excluded_indexes);
  pthread_exit(NULL);
}

static struct Mapping *solver_function(const struct Interval *interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }
  pthread_barrier_init(&barrier, NULL, THREAD_NUMBER);
  pthread_mutex_init(&mutex, NULL);

  const struct ThreadInputContext context = {
      .interval = interval,
      .mapping = malloc(sizeof(struct Mapping)),
      .chains = malloc(sizeof(struct AlternatingChains)),
      .exclusion_array = malloc(interval->size * sizeof(bool)),
      .counts = malloc(sizeof(struct IntervalCounts)),
  };

  context.counts->source_num = 0;
  context.counts->target_num = 0;
  memset(context.exclusion_array, 0, context.interval->size);

  struct ThreadsConfig thread_config = {
      .ids = malloc(THREAD_NUMBER * sizeof(pthread_t)),
      .inputs = malloc(THREAD_NUMBER * sizeof(struct ThreadInput)),
  };
  for (int i = 0; i < THREAD_NUMBER; i++) {
    thread_config.inputs[i].context = context;
    thread_config.inputs[i].thread_index = i;
    pthread_create(&thread_config.ids[i], NULL, solve_interval_range,
                   &thread_config.inputs[i]);
  }

  for (int i = 0; i < THREAD_NUMBER; i++) {
    pthread_join(thread_config.ids[i], NULL);
  }

  thread_config_free(thread_config);
  alternating_chains_free(context.chains);

  free(context.exclusion_array);
  free(context.counts);

  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&mutex);

  return context.mapping;
}

const struct Solver aggarwal_parallel_solver = {
    .solve = solver_function,
    .name = "Aggarwal solver parallel",
};
