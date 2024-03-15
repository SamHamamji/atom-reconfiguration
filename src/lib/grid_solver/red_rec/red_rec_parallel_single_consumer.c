#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid_solver.h"
#include "./red_rec_utils/red_rec_utils.h"

struct ThreadInputContext {
  struct Grid *grid;
  struct Reconfiguration *reconfiguration;
  struct Range target_range;
};

struct ThreadSyncVariables {
  pthread_barrier_t *barrier;
  pthread_mutex_t *total_counts_mutex;
  pthread_mutex_t *reconfiguration_mutex;
  sem_t *delayed_moves_semaphore;
  sem_t *solving_receivers_semaphore;
};

struct ThreadSharedVariables {
  struct Counts *column_counts;
  struct Counts *total_counts;
  struct DelayedMoves delayed_moves;
  struct ReceiverOrder *receiver_order;
};

struct ThreadInput {
  struct ThreadInputContext context;
  struct ThreadSyncVariables sync;
  struct ThreadSharedVariables shared;
  const RedRecParallelParams *params;
  int thread_index;
};

static struct ThreadSyncVariables thread_sync_variables_new(int thread_num) {
  struct ThreadSyncVariables sync = {
      .barrier = malloc(sizeof(pthread_barrier_t)),
      .reconfiguration_mutex = malloc(sizeof(pthread_mutex_t)),
      .total_counts_mutex = malloc(sizeof(pthread_mutex_t)),
      .delayed_moves_semaphore = malloc(sizeof(sem_t)),
      .solving_receivers_semaphore = malloc(sizeof(sem_t)),
  };
  pthread_barrier_init(sync.barrier, NULL, thread_num);
  pthread_mutex_init(sync.reconfiguration_mutex, NULL);
  pthread_mutex_init(sync.total_counts_mutex, NULL);
  sem_init(sync.delayed_moves_semaphore, 0, 0);
  sem_init(sync.solving_receivers_semaphore, 0, 0);
  return sync;
}

static void thread_sync_variables_free(struct ThreadSyncVariables sync) {
  pthread_barrier_destroy(sync.barrier);
  pthread_mutex_destroy(sync.reconfiguration_mutex);
  pthread_mutex_destroy(sync.total_counts_mutex);
  sem_destroy(sync.delayed_moves_semaphore);
  sem_destroy(sync.solving_receivers_semaphore);
  free(sync.barrier);
  free(sync.reconfiguration_mutex);
  free(sync.total_counts_mutex);
  free(sync.delayed_moves_semaphore);
  free(sync.solving_receivers_semaphore);
}

static void thread_shared_variables_free(struct ThreadSharedVariables shared) {
  free(shared.column_counts);
  free(shared.total_counts);
  delayed_moves_free(shared.delayed_moves);
  receiver_order_free(shared.receiver_order);
}

static void solve_receiver(struct Grid *grid,
                           struct Reconfiguration *reconfiguration,
                           struct ReceiverDelayedMoves delayed_moves,
                           struct Range target_range,
                           const RedRecParallelParams *params) {
  int receiver_pivot = get_receiver_pivot(grid, delayed_moves, target_range,
                                          params->linear_solver);

  struct Range *fixed_sources_range = &(struct Range){
      .start = receiver_pivot,
      .exclusive_end = receiver_pivot,
  };

  int receiver_index = delayed_moves.pairs[0].receiver_index;
  execute_move(grid, reconfiguration, fixed_sources_range,
               (struct ColumnPair){
                   .donor_index = receiver_index,
                   .receiver_index = receiver_index,
               });

  for (int i = 0; i < delayed_moves.length; i++) {
    execute_move(grid, reconfiguration, fixed_sources_range,
                 delayed_moves.pairs[i]);
  }
}

static void execute_delayed_moves(struct ThreadInput *input) {
  sem_wait(input->sync.delayed_moves_semaphore);

  for (int i = 0; i < input->shared.receiver_order->receiver_num; i++) {
    solve_receiver(
        input->context.grid, input->context.reconfiguration,
        input->shared.delayed_moves
            .array[input->shared.receiver_order->receiver_indexes[i]],
        input->context.target_range, input->params);
    sem_wait(input->sync.delayed_moves_semaphore);
  }
}

static void *red_rec_parallel_thread(void *thread_input) {
  struct ThreadInput *input = (struct ThreadInput *)thread_input;

  // Step 1: Compute counts
  struct Range column_range =
      get_range(input->thread_index, input->params->thread_num,
                input->context.grid->width);

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    input->shared.column_counts[i] = interval_get_counts(&(struct Interval){
        .array = grid_get_column(input->context.grid, i),
        .length = input->context.grid->height,
    });

    pthread_mutex_lock(input->sync.total_counts_mutex);
    input->shared.total_counts->source_num +=
        input->shared.column_counts[i].source_num;
    input->shared.total_counts->target_num +=
        input->shared.column_counts[i].target_num;
    pthread_mutex_unlock(input->sync.total_counts_mutex);
  }

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    if (counts_get_imbalance(input->shared.column_counts[i]) >= 0) {
      struct Mapping *mapping = input->params->linear_solver->solve(
          &(const struct Interval){
              .array = grid_get_column(input->context.grid, i),
              .length = input->context.grid->height,
          },
          input->params->linear_solver->params);

      pthread_mutex_lock(input->sync.reconfiguration_mutex);
      reconfiguration_add_mapping(input->context.reconfiguration,
                                  input->context.grid, mapping, i);
      pthread_mutex_unlock(input->sync.reconfiguration_mutex);

      mapping_free(mapping);
    }
  }

  pthread_barrier_wait(input->sync.barrier);

  if (input->thread_index == input->params->thread_num - 1) {
    sem_post(input->sync.solving_receivers_semaphore);
  }

  int total_imbalance = counts_get_imbalance(*input->shared.total_counts);
  if (total_imbalance < 0) {
    pthread_exit(NULL);
  }

  if (input->thread_index == input->params->thread_num - 1) {
    execute_delayed_moves(input);
  }

  pthread_exit(NULL);
}

static void produce_delayed_moves(struct Grid *grid,
                                  struct Counts *column_counts,
                                  struct ReceiverOrder *receiver_order,
                                  struct DelayedMoves delayed_moves,
                                  sem_t *delayed_moves_semaphore) {
  struct ColumnPair best_pair = column_pair_get_best(grid, column_counts);
  while (column_pair_exists(best_pair)) {
    delayed_moves_add(delayed_moves, best_pair);
    if (best_pair.exchanged_sources_num == best_pair.receiver_deficit) {
      receiver_order_push(receiver_order, best_pair.receiver_index);
      sem_post(delayed_moves_semaphore);
    }

    column_counts[best_pair.donor_index].source_num -=
        best_pair.exchanged_sources_num;
    column_counts[best_pair.receiver_index].source_num +=
        best_pair.exchanged_sources_num;

    best_pair = column_pair_get_best(grid, column_counts);
  }
  sem_post(delayed_moves_semaphore);
}

struct Reconfiguration *red_rec_parallel_single_consumer(struct Grid *grid,
                                                         const void *params) {
  assert(grid_target_region_is_compact(grid));
  assert(params != NULL);

  int thread_num = ((RedRecParallelParams *)params)->thread_num;
  assert(thread_num > 0);

  struct ThreadInputContext context = {
      .grid = grid,
      .reconfiguration = reconfiguration_new(2 * grid->width * grid->height),
      .target_range = grid_get_compact_target_region_range(grid),
  };

  struct ThreadSharedVariables shared = {
      .column_counts = malloc(grid->width * sizeof(struct Counts)),
      .total_counts = malloc(sizeof(struct Counts)),
      .delayed_moves = delayed_moves_new(grid),
      .receiver_order = receiver_order_new(grid->width),
  };
  *shared.total_counts = (struct Counts){0, 0};
  struct ThreadSyncVariables sync = thread_sync_variables_new(thread_num);

  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));
  pthread_t *thread_ids = malloc(thread_num * sizeof(pthread_t));

  for (int i = 0; i < thread_num; i++) {
    thread_inputs[i] = (struct ThreadInput){
        .context = context,
        .shared = shared,
        .sync = sync,
        .params = params,
        .thread_index = i,
    };

    pthread_create(&thread_ids[i], NULL, red_rec_parallel_thread,
                   &thread_inputs[i]);
  }

  for (int i = 0; i < thread_num - 1; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  sem_wait(sync.solving_receivers_semaphore);

  if (counts_get_imbalance(*shared.total_counts) < 0) {
    pthread_join(thread_ids[thread_num - 1], NULL);

    thread_sync_variables_free(sync);
    thread_shared_variables_free(shared);
    reconfiguration_free(context.reconfiguration);
    free(thread_inputs);
    free(thread_ids);

    return NULL;
  }

  produce_delayed_moves(grid, shared.column_counts, shared.receiver_order,
                        shared.delayed_moves, sync.delayed_moves_semaphore);

  pthread_join(thread_ids[thread_num - 1], NULL);

  thread_sync_variables_free(sync);
  thread_shared_variables_free(shared);
  free(thread_inputs);
  free(thread_ids);

  reconfiguration_filter_identical(context.reconfiguration);

  return context.reconfiguration;
}
