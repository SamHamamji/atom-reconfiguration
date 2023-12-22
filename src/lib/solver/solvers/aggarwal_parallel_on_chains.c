#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../solver.h"

#define THREAD_NUMBER 8

struct ThreadInputContext {
  const struct AlternatingChains *chains;
  int *output;
  int interval_size;
};

struct ThreadInput {
  struct ThreadInputContext context;
  struct ChainRange chain_range;
};

static void *get_exclusion_from_chain_range(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;
  int *excluded_indexes = alternating_chains_get_exclusion_from_range(
      input->context.chains, input->chain_range, input->context.interval_size);

  // Reduces false sharing
  memcpy(
      &input->context.output[input->chain_range.min_chain], excluded_indexes,
      (input->chain_range.max_chain_exclusive - input->chain_range.min_chain) *
          sizeof(int));

  free(excluded_indexes);
  pthread_exit(NULL);
}

static bool *get_exclusion_from_chains(const struct AlternatingChains *chains,
                                       int interval_size, int imbalance) {
  int *excluded_indexes = malloc(sizeof(int) * imbalance);
  pthread_t *thread_array = malloc(THREAD_NUMBER * sizeof(pthread_t));
  struct ThreadInput *thread_inputs =
      malloc(THREAD_NUMBER * sizeof(struct ThreadInput));

  const int heights_per_thread = imbalance / THREAD_NUMBER;
  const int remaining_heights = imbalance % THREAD_NUMBER;
  const struct ThreadInputContext context = {
      .chains = chains,
      .output = excluded_indexes,
      .interval_size = interval_size,
  };

  for (int i = 0; i < THREAD_NUMBER; i++) {
    thread_inputs[i].chain_range.min_chain =
        (i == 0 ? 0 : thread_inputs[i - 1].chain_range.max_chain_exclusive);
    thread_inputs[i].chain_range.max_chain_exclusive =
        thread_inputs[i].chain_range.min_chain + heights_per_thread +
        (int)(i < remaining_heights);
    thread_inputs[i].context = context;

    pthread_create(&thread_array[i], NULL, get_exclusion_from_chain_range,
                   &thread_inputs[i]);
  }

  bool *exclusion_array = calloc(interval_size, sizeof(bool));
  for (int i = 0; i < THREAD_NUMBER; i++) {
    pthread_join(thread_array[i], NULL);
    for (int height = thread_inputs[i].chain_range.min_chain;
         height < thread_inputs[i].chain_range.max_chain_exclusive; height++) {
      exclusion_array[excluded_indexes[height]] = true;
    }
  }

  free(thread_inputs);
  free(thread_array);
  free(excluded_indexes);
  return exclusion_array;
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
      get_exclusion_from_chains(chains, interval->size, imbalance);
  alternating_chains_free(chains);

  struct Mapping *mapping =
      solve_neutral_interval(interval, exclusion_array, counts.target_num);
  free(exclusion_array);

  return mapping;
}

const struct Solver aggarwal_parallel_solver_on_chains = {
    .solve = solver_function,
    .name = "Aggarwal solver parallel on chains",
};
