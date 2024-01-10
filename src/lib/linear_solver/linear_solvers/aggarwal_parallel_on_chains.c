#include <assert.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../linear_solver.h"

struct ThreadInputContext {
  const struct AlternatingChains *chains;
  int *output;
  int interval_length;
};

struct ThreadInput {
  struct ThreadInputContext context;
  struct Range chain_range;
};

static void *get_exclusion_from_chain_range(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;
  int *excluded_indexes = alternating_chains_get_exclusion_from_range(
      input->context.chains, input->chain_range,
      input->context.interval_length);

  // Reduces false sharing
  memcpy(&input->context.output[input->chain_range.start], excluded_indexes,
         (input->chain_range.exclusive_end - input->chain_range.start) *
             sizeof(int));

  free(excluded_indexes);
  pthread_exit(NULL);
}

static bool *get_exclusion_from_chains(const struct AlternatingChains *chains,
                                       int thread_num, int interval_length,
                                       int imbalance) {
  int *excluded_indexes = malloc(sizeof(int) * imbalance);
  pthread_t *thread_array = malloc(thread_num * sizeof(pthread_t));
  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));

  const int heights_per_thread = imbalance / thread_num;
  const int remaining_heights = imbalance % thread_num;
  const struct ThreadInputContext context = {
      .chains = chains,
      .output = excluded_indexes,
      .interval_length = interval_length,
  };

  for (int i = 0; i < thread_num; i++) {
    thread_inputs[i].chain_range = get_range(i, thread_num, imbalance);
    thread_inputs[i].context = context;

    pthread_create(&thread_array[i], NULL, get_exclusion_from_chain_range,
                   &thread_inputs[i]);
  }

  bool *exclusion_array = calloc(interval_length, sizeof(bool));
  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_array[i], NULL);
    for (int height = thread_inputs[i].chain_range.start;
         height < thread_inputs[i].chain_range.exclusive_end; height++) {
      exclusion_array[excluded_indexes[height]] = true;
    }
  }

  free(thread_inputs);
  free(thread_array);
  free(excluded_indexes);
  return exclusion_array;
}

struct Mapping *
linear_solve_aggarwal_parallel_on_chains(const struct Interval *interval,
                                         const void *params) {
  assert(params != NULL);
  int thread_num = ((AggarwalParallelOnChainsParams *)params)->thread_num;
  if (interval->length <= 0) {
    return mapping_get_null();
  }

  struct Counts counts = interval_get_counts(interval);
  int imbalance = counts_get_imbalance(counts);

  if (imbalance < 0) {
    return mapping_get_null();
  }

  struct AlternatingChains *chains =
      alternating_chains_get(interval, imbalance);

  bool *exclusion_array = get_exclusion_from_chains(
      chains, thread_num, interval->length, imbalance);
  alternating_chains_free(chains);

  struct Mapping *mapping =
      solve_neutral_interval(interval, exclusion_array, counts.target_num);
  free(exclusion_array);

  return mapping;
}
