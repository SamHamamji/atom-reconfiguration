#include <assert.h>
#include <pthread.h>
#include <stdlib.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../linear_solver.h"

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
  solve_neutral_interval_half(
      input->context.interval, input->context.exclusion_array,
      input->context.mapping, (bool)(input->thread_index % 2),
      (bool)(input->thread_index / 2));
  pthread_exit(NULL);
}

static struct Mapping *
get_mapping_from_exclusion_array(const struct Interval *interval,
                                 const bool *exclusion_array, int target_num,
                                 int thread_num) {
  pthread_t *thread_array = malloc(thread_num * sizeof(pthread_t));
  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));

  const struct ThreadInputContext context = {
      .interval = interval,
      .exclusion_array = exclusion_array,
      .mapping = mapping_new(target_num),
  };

  for (int i = 0; i < thread_num; i++) {
    thread_inputs[i].context = context;
    thread_inputs[i].thread_index = i;

    pthread_create(&thread_array[i], NULL, compute_mapping_from_exclusion_array,
                   &thread_inputs[i]);
  }

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_array[i], NULL);
  }

  free(thread_inputs);
  free(thread_array);

  return context.mapping;
}

struct Mapping *
linear_solve_aggarwal_parallel_on_neutral(const struct Interval *interval,
                                          const void *params) {
  assert(params != NULL);
  int thread_num = ((AggarwalParallelOnNeutralParams *)params)->thread_num;
  assert(thread_num >= 4);

  struct Counts counts = interval_get_counts(interval);
  int imbalance = counts_get_imbalance(counts);
  if (imbalance < 0) {
    return NULL;
  }

  struct AlternatingChains *chains =
      alternating_chains_get(interval, imbalance);

  bool *exclusion_array =
      alternating_chains_get_exclusion_array(chains, interval->length);
  alternating_chains_free(chains);

  struct Mapping *mapping = get_mapping_from_exclusion_array(
      interval, exclusion_array, counts.target_num, thread_num);
  free(exclusion_array);

  return mapping;
}
