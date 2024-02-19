#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdlib.h>
#include <string.h>

#include "../../interval/interval.h"
#include "../common/alternating_chains.h"
#include "../common/solve_neutral_interval.h"
#include "../linear_solver.h"

struct ThreadInputContext {
  const struct Interval *interval;
  struct Mapping *mapping;
  int thread_num;
};

struct ThreadSyncVariables {
  pthread_barrier_t *barrier;
  pthread_mutex_t *mutex;
  sem_t *semaphores;
};

struct ThreadSharedVariables {
  struct Counts *thread_counts_array;
  struct Counts *total_counts;
  bool *exclusion_array;
  struct ThreadIndexes {
    int first_source_index;
    int first_target_index;
  } *first_indexes;
};

struct ThreadInput {
  struct ThreadInputContext *context;
  struct ThreadSyncVariables *sync;
  struct ThreadSharedVariables *shared;
  int thread_index;
};

static void thread_sync_variables_free(struct ThreadSyncVariables *sync,
                                       int thread_num) {
  for (int i = 0; i < thread_num; i++) {
    sem_destroy(&sync->semaphores[i]);
  }
  pthread_barrier_destroy(sync->barrier);
  pthread_mutex_destroy(sync->mutex);
  free(sync->barrier);
  free(sync->mutex);
  free(sync->semaphores);
}

static void thread_shared_variables_free(struct ThreadSharedVariables *shared) {
  free(shared->exclusion_array);
  free(shared->first_indexes);
  free(shared->thread_counts_array);
  free(shared->total_counts);
}

static void compute_interval_counts(const struct Interval *interval,
                                    struct Range interval_range,
                                    struct Counts *range_counts,
                                    struct Counts *total_counts,
                                    pthread_mutex_t *mutex) {
  *range_counts = interval_get_counts_from_range(interval, interval_range);
  pthread_mutex_lock(mutex);
  total_counts->source_num += range_counts->source_num;
  total_counts->target_num += range_counts->target_num;
  pthread_mutex_unlock(mutex);
}

static void *solve_interval_range(void *args) {
  // Step 1: Compute counts
  struct ThreadInput *input = (struct ThreadInput *)args;

  struct Range interval_range =
      get_range(input->thread_index, input->context->thread_num,
                input->context->interval->length);

  compute_interval_counts(
      input->context->interval, interval_range,
      &input->shared->thread_counts_array[input->thread_index],
      input->shared->total_counts, input->sync->mutex);

  pthread_barrier_wait(input->sync->barrier);

  // Step 2: Compute chains
  int imbalance = counts_get_imbalance(*input->shared->total_counts);

  if (imbalance < 0) {
    pthread_exit(NULL);
  }

  struct Range chain_range =
      get_range(input->thread_index, input->context->thread_num, imbalance);

  struct AlternatingChains chains = {
      .chain_start_indexes = malloc(imbalance * sizeof(int)),
      .right_partners = malloc(input->context->interval->length * sizeof(int)),
      .chain_num = imbalance,
  };

  alternating_chains_compute_range(input->context->interval, &chains,
                                   chain_range);

  // Step 3: Compute exclusion array from chains
  int *excluded_indexes = alternating_chains_get_exclusion_from_range(
      &chains, chain_range, input->context->interval->length);

  free(chains.chain_start_indexes);
  free(chains.right_partners);

  const int chain_range_length = chain_range.exclusive_end - chain_range.start;
  for (int i = 0; i < chain_range_length; i++) {
    input->shared->exclusion_array[excluded_indexes[i]] = true;
  }

  pthread_mutex_lock(input->sync->mutex);
  for (int i = 0; i < chain_range_length; i++) {
    int excluded_source_range =
        get_range_index(excluded_indexes[i], input->context->thread_num,
                        input->context->interval->length);

    if (excluded_source_range != input->context->thread_num - 1) {
      input->shared->first_indexes[excluded_source_range + 1]
          .first_source_index--;
    }
  }
  pthread_mutex_unlock(input->sync->mutex);
  free(excluded_indexes);

  if (input->thread_index == 0) {
    input->context->mapping =
        mapping_new(input->shared->total_counts->target_num);
  }

  pthread_barrier_wait(input->sync->barrier);

  // Step 4: Get mapping from exclusion
  if (input->thread_index == input->context->thread_num - 1) {
    for (int i = 1; i < input->context->thread_num; i++) {
      input->shared->first_indexes[i].first_source_index +=
          input->shared->first_indexes[i - 1].first_source_index +
          input->shared->thread_counts_array[i - 1].source_num;
      input->shared->first_indexes[i].first_target_index +=
          input->shared->first_indexes[i - 1].first_target_index +
          input->shared->thread_counts_array[i - 1].target_num;
      sem_post(&input->sync->semaphores[i]);
    }
  } else if (input->thread_index != 0) {
    sem_wait(&input->sync->semaphores[input->thread_index]);
  }

  solve_neutral_interval_range(
      input->context->interval, input->shared->exclusion_array, interval_range,
      input->shared->first_indexes[input->thread_index].first_source_index,
      input->shared->first_indexes[input->thread_index].first_target_index,
      input->context->mapping);

  pthread_exit(NULL);
}

struct Mapping *linear_solve_aggarwal_parallel(const struct Interval *interval,
                                               const void *params) {
  assert(params != NULL);

  int thread_num = ((AggarwalParallelParams *)params)->thread_num;

  struct ThreadInputContext context = (struct ThreadInputContext){
      .interval = interval,
      .mapping = NULL,
      .thread_num = thread_num,
  };

  struct ThreadSyncVariables sync = (struct ThreadSyncVariables){
      .semaphores = malloc(thread_num * sizeof(sem_t)),
      .mutex = malloc(sizeof(pthread_mutex_t)),
      .barrier = malloc(sizeof(pthread_barrier_t)),
  };

  pthread_barrier_init(sync.barrier, NULL, thread_num);
  pthread_mutex_init(sync.mutex, NULL);

  struct ThreadSharedVariables shared = (struct ThreadSharedVariables){
      .exclusion_array = malloc(interval->length * sizeof(bool)),
      .first_indexes = malloc(thread_num * sizeof(struct ThreadIndexes)),
      .total_counts = malloc(sizeof(struct Counts)),
      .thread_counts_array = malloc(thread_num * sizeof(struct Counts)),
  };

  *shared.total_counts = (struct Counts){0, 0};
  memset(shared.first_indexes, 0, thread_num * sizeof(struct ThreadIndexes));
  memset(shared.exclusion_array, 0, interval->length);

  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));
  pthread_t *thread_ids = malloc(thread_num * sizeof(pthread_t));

  for (int i = 0; i < thread_num; i++) {
    sem_init(&sync.semaphores[i], false, 0);
    thread_inputs[i] = (struct ThreadInput){
        .context = &context,
        .sync = &sync,
        .shared = &shared,
        .thread_index = i,
    };
    pthread_create(&thread_ids[i], NULL, solve_interval_range,
                   &thread_inputs[i]);
  }

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  free(thread_ids);
  free(thread_inputs);

  thread_sync_variables_free(&sync, thread_num);
  thread_shared_variables_free(&shared);

  return context.mapping;
}
