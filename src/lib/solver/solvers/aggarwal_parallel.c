#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
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
  struct Mapping *mapping;
  struct IntervalCounts *thread_counts_array;
  struct IntervalCounts *global_counts;
  bool *exclusion_array;
  struct ThreadIndexes {
    int first_source_index;
    int first_target_index;
  } *first_indexes;
  sem_t *semaphores;
  int thread_num;
};

struct ThreadInput {
  struct ThreadInputContext context;
  int thread_index;
};

static void thread_context_free(struct ThreadInputContext context) {
  free(context.exclusion_array);
  free(context.global_counts);
  free(context.thread_counts_array);
  free(context.first_indexes);
  free(context.semaphores);
}

static void compute_interval_counts(const struct Interval *interval,
                                    struct Range interval_range,
                                    struct IntervalCounts *range_counts,
                                    struct IntervalCounts *global_counts) {
  *range_counts = interval_get_counts_from_range(interval, interval_range);
  pthread_mutex_lock(&mutex);
  global_counts->source_num += range_counts->source_num;
  global_counts->target_num += range_counts->target_num;
  pthread_mutex_unlock(&mutex);
}

static void *solve_interval_range(void *args) {
  const struct ThreadInput *input = (struct ThreadInput *)args;

  struct Range interval_range =
      get_range(input->thread_index, input->context.thread_num,
                input->context.interval->length);

  compute_interval_counts(
      input->context.interval, interval_range,
      &input->context.thread_counts_array[input->thread_index],
      input->context.global_counts);

  pthread_barrier_wait(&barrier);

  int imbalance = get_imbalance_from_counts(*input->context.global_counts);
  bool solvable = (bool)(imbalance >= 0);

  if (input->thread_index == 0) {
    int pair_count = solvable ? input->context.global_counts->target_num : 0;
    *input->context.mapping = (struct Mapping){
        .pairs = malloc(pair_count * sizeof(struct Pair)),
        .pair_count = pair_count,
    };
  }

  if (!solvable) {
    pthread_exit(NULL);
  }

  struct Range chain_range =
      get_range(input->thread_index, input->context.thread_num, imbalance);

  struct AlternatingChains chains = {
      .chain_start_indexes = malloc(imbalance * sizeof(int)),
      .right_partners = malloc(input->context.interval->length * sizeof(int)),
  };

  alternating_chains_compute_range(input->context.interval, &chains,
                                   chain_range);

  int *excluded_indexes = alternating_chains_get_exclusion_from_range(
      &chains, chain_range, input->context.interval->length);

  free(chains.chain_start_indexes);
  free(chains.right_partners);

  const int chain_range_length = chain_range.exclusive_end - chain_range.start;
  for (int i = 0; i < chain_range_length; i++) {
    input->context.exclusion_array[excluded_indexes[i]] = true;
  }

  pthread_mutex_lock(&mutex);
  for (int i = 0; i < chain_range_length; i++) {
    int excluded_source_range =
        get_range_index(excluded_indexes[i], input->context.thread_num,
                        input->context.interval->length);

    if (excluded_source_range != input->context.thread_num - 1) {
      input->context.first_indexes[excluded_source_range + 1]
          .first_source_index--;
    }
  }
  pthread_mutex_unlock(&mutex);
  free(excluded_indexes);

  pthread_barrier_wait(&barrier);

  if (input->thread_index == input->context.thread_num - 1) {
    for (int i = 1; i < input->context.thread_num; i++) {
      input->context.first_indexes[i].first_source_index +=
          input->context.first_indexes[i - 1].first_source_index +
          input->context.thread_counts_array[i - 1].source_num;
      input->context.first_indexes[i].first_target_index +=
          input->context.first_indexes[i - 1].first_target_index +
          input->context.thread_counts_array[i - 1].target_num;
      sem_post(&input->context.semaphores[i]);
    }
  } else if (input->thread_index != 0) {
    sem_wait(&input->context.semaphores[input->thread_index]);
  }

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
  if (interval->length <= 0) {
    return mapping_get_null();
  }

  int thread_num = ((AggarwalParallelParams *)params)->thread_num;
  pthread_barrier_init(&barrier, NULL, thread_num);
  pthread_mutex_init(&mutex, NULL);

  const struct ThreadInputContext context = {
      .interval = interval,
      .mapping = malloc(sizeof(struct Mapping)),
      .exclusion_array = malloc(interval->length * sizeof(bool)),
      .global_counts = malloc(sizeof(struct IntervalCounts)),
      .thread_counts_array = malloc(thread_num * sizeof(struct IntervalCounts)),
      .first_indexes = malloc(thread_num * sizeof(struct ThreadIndexes)),
      .semaphores = malloc(thread_num * sizeof(pthread_mutex_t)),
      .thread_num = thread_num,
  };

  *context.global_counts = (struct IntervalCounts){0, 0};
  memset(context.first_indexes, 0, thread_num * sizeof(struct ThreadIndexes));
  memset(context.exclusion_array, 0, context.interval->length);

  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));
  pthread_t *thread_ids = malloc(thread_num * sizeof(pthread_t));

  for (int i = 0; i < thread_num; i++) {
    sem_init(&context.semaphores[i], false, 0);
    thread_inputs[i].context = context;
    thread_inputs[i].thread_index = i;
    pthread_create(&thread_ids[i], NULL, solve_interval_range,
                   &thread_inputs[i]);
  }

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_ids[i], NULL);
    sem_destroy(&context.semaphores[i]);
  }
  pthread_barrier_destroy(&barrier);
  pthread_mutex_destroy(&mutex);

  thread_context_free(context);
  free(thread_ids);
  free(thread_inputs);

  return context.mapping;
}
