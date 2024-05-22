#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid_solver.h"
#include "./red_rec_parallel_utils/red_rec_parallel_utils.h"
#include "./red_rec_utils/red_rec_utils.h"

struct ThreadSyncVariables {
  pthread_mutex_t *reconfiguration_mutex;
};

struct ThreadSharedVariables {
  struct Counts *column_counts;
  struct Counts *total_counts;
};

struct ThreadInput {
  struct RedRecThreadInputContext context;
  struct ThreadSyncVariables sync;
  struct ThreadSharedVariables shared;
  const RedRecParallelParams *params;
  int thread_index;
};

static void thread_sync_variables_free(struct ThreadSyncVariables sync) {
  pthread_mutex_destroy(sync.reconfiguration_mutex);
  free(sync.reconfiguration_mutex);
}

static void thread_shared_variables_free(struct ThreadSharedVariables shared) {
  free(shared.column_counts);
  free(shared.total_counts);
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

static void *red_rec_parallel_thread(void *thread_input) {
  struct ThreadInput *input = (struct ThreadInput *)thread_input;

  struct Range column_range =
      get_range(input->thread_index, input->params->thread_num,
                input->context.grid->width);

  compute_counts_parallel(input->context.grid,
                          &input->shared.column_counts[column_range.start],
                          input->shared.total_counts,
                          input->sync.reconfiguration_mutex, column_range);

  solve_donors_parallel(input->context,
                        &input->shared.column_counts[column_range.start],
                        input->params->linear_solver,
                        input->sync.reconfiguration_mutex, column_range);

  pthread_exit(NULL);
}

static void solver_receiver_columns(struct Grid *grid,
                                    struct Reconfiguration *reconfiguration,
                                    struct Counts *column_counts,
                                    struct Range target_range,
                                    const RedRecParallelParams *params) {
  struct DelayedMoves delayed_moves = delayed_moves_new(grid);
  struct ColumnPairPQ column_pair_pq =
      column_pair_pq_new(column_counts, grid->width);

  struct ColumnPair best_pair = column_pair_pq_pop(&column_pair_pq);
  while (!column_pair_pq_is_empty(&column_pair_pq)) {
    delayed_moves_add(delayed_moves, best_pair);
    if (get_exchange_num(best_pair) == -best_pair.receiver_deficit) {
      solve_receiver(grid, reconfiguration,
                     delayed_moves.array[best_pair.receiver_index],
                     target_range, params);
    }

    best_pair = column_pair_pq_pop(&column_pair_pq);
  }

  column_pair_pq_free(&column_pair_pq);
  delayed_moves_free(delayed_moves);
}

struct Reconfiguration *red_rec_parallel(struct Grid *grid,
                                         const void *params) {
  assert(grid_target_region_is_compact(grid));
  assert(params != NULL);

  int thread_num = ((RedRecParallelParams *)params)->thread_num;
  assert(thread_num > 0);

  struct ThreadSyncVariables sync = {
      .reconfiguration_mutex = malloc(sizeof(pthread_mutex_t)),
  };
  pthread_mutex_init(sync.reconfiguration_mutex, NULL);

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
  };
  *shared.total_counts = (struct Counts){0, 0};

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

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  free(thread_inputs);
  free(thread_ids);

  if (counts_get_imbalance(*shared.total_counts) < 0) {
    thread_sync_variables_free(sync);
    thread_shared_variables_free(shared);
    reconfiguration_free(context.reconfiguration);

    return NULL;
  }

  solver_receiver_columns(grid, context.reconfiguration, shared.column_counts,
                          context.target_range, params);

  reconfiguration_filter_identical(context.reconfiguration);

  thread_sync_variables_free(sync);
  thread_shared_variables_free(shared);

  return context.reconfiguration;
}
