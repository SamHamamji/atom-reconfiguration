#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid_solver.h"
#include "./red_rec_parallel_utils/red_rec_parallel_utils.h"
#include "./red_rec_utils/red_rec_utils.h"

struct ThreadSyncVariables {
  pthread_barrier_t *barrier;
  pthread_mutex_t *reconfiguration_mutex;
  sem_t *delayed_moves_semaphore;
};

struct ThreadSharedVariables {
  struct Counts *column_counts;
  struct Counts *total_counts;
  struct DelayedMoves delayed_moves;
  struct ReceiverOrder *receiver_order;
};

struct ThreadInput {
  struct RedRecThreadInputContext context;
  struct ThreadSyncVariables sync;
  struct ThreadSharedVariables shared;
  const RedRecParallelParams *params;
  int thread_index;
};

static struct ThreadSyncVariables thread_sync_variables_new(int thread_num) {
  struct ThreadSyncVariables sync = {
      .barrier = malloc(sizeof(pthread_barrier_t)),
      .reconfiguration_mutex = malloc(sizeof(pthread_mutex_t)),
      .delayed_moves_semaphore = malloc(sizeof(sem_t)),
  };
  pthread_barrier_init(sync.barrier, NULL, thread_num + 1);
  pthread_mutex_init(sync.reconfiguration_mutex, NULL);
  sem_init(sync.delayed_moves_semaphore, 0, 0);
  return sync;
}

static void thread_sync_variables_free(struct ThreadSyncVariables sync) {
  pthread_barrier_destroy(sync.barrier);
  pthread_mutex_destroy(sync.reconfiguration_mutex);
  sem_destroy(sync.delayed_moves_semaphore);
  free(sync.barrier);
  free(sync.reconfiguration_mutex);
  free(sync.delayed_moves_semaphore);
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

  struct Range column_range =
      get_range(input->thread_index, input->params->thread_num,
                input->context.grid->width);

  compute_counts_and_solve_donors_parallel(
      input->context, input->shared.column_counts, input->shared.total_counts,
      input->params->linear_solver, input->sync.reconfiguration_mutex,
      column_range);

  pthread_barrier_wait(input->sync.barrier);

  if (input->thread_index == 0 &&
      counts_get_imbalance(*input->shared.total_counts) >= 0) {
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

  struct RedRecThreadInputContext context = {
      .grid = grid,
      .target_range = grid_get_compact_target_region_range(grid),
  };
  context.reconfiguration = reconfiguration_new(
      2 * grid->width *
      (context.target_range.exclusive_end - context.target_range.start));

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

  pthread_barrier_wait(sync.barrier);

  if (counts_get_imbalance(*shared.total_counts) < 0) {
    for (int i = 0; i < thread_num; i++) {
      pthread_join(thread_ids[i], NULL);
    }

    thread_sync_variables_free(sync);
    thread_shared_variables_free(shared);
    reconfiguration_free(context.reconfiguration);
    free(thread_inputs);
    free(thread_ids);

    return NULL;
  }

  produce_delayed_moves(grid, shared.column_counts, shared.receiver_order,
                        shared.delayed_moves, sync.delayed_moves_semaphore);

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  thread_sync_variables_free(sync);
  thread_shared_variables_free(shared);
  free(thread_inputs);
  free(thread_ids);

  reconfiguration_filter_identical(context.reconfiguration);

  return context.reconfiguration;
}
