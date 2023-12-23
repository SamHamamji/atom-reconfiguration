#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

#define THREAD_NUMBER 4

struct ThreadInputContext {
  const struct Interval *interval;
  const bool *exclusion_array;
  struct Mapping *mapping;
};

struct ThreadInput {
  struct ThreadInputContext context;
  int thread_index;
};

static void *compute_mapping_from_exclusion_array(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;
  solve_neutral_interval_slice(
      input->context.interval, input->context.exclusion_array,
      input->context.mapping, (bool)(input->thread_index % 2),
      (bool)(input->thread_index / 2));
  pthread_exit(NULL);
}

static struct Mapping *solver_function(const struct Interval *interval) {
  if (interval->size <= 0) {
    return mapping_get_null();
  }

  struct IntervalCounts counts = interval_get_counts(interval);
  int imbalance = get_imbalance_from_counts(counts);
  if (imbalance < 0) {
    return mapping_get_null();
  }

  struct AlternatingChains *chains =
      alternating_chains_get(interval, imbalance);

  bool *exclusion_array =
      alternating_chains_get_exclusion_array(chains, interval->size, imbalance);
  alternating_chains_free(chains);

  pthread_t *thread_array = malloc(THREAD_NUMBER * sizeof(pthread_t));
  struct ThreadInput *thread_inputs =
      malloc(THREAD_NUMBER * sizeof(struct ThreadInput));

  const struct ThreadInputContext context = {
      .interval = interval,
      .exclusion_array = exclusion_array,
      .mapping = malloc(sizeof(struct Mapping)),
  };

  context.mapping->pairs = malloc(counts.target_num * sizeof(struct Pair));
  context.mapping->pair_count = counts.target_num;

  for (int i = 0; i < THREAD_NUMBER; i++) {
    thread_inputs[i].context = context;
    thread_inputs[i].thread_index = i;

    pthread_create(&thread_array[i], NULL, compute_mapping_from_exclusion_array,
                   &thread_inputs[i]);
  }

  for (int i = 0; i < THREAD_NUMBER; i++) {
    pthread_join(thread_array[i], NULL);
  }

  free(exclusion_array);
  free(thread_inputs);
  free(thread_array);

  return context.mapping;
}

const struct Solver aggarwal_parallel_solver_on_neutral = {
    .solve = solver_function,
    .name = "Aggarwal solver parallel on neutral",
};
