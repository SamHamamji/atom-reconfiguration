#include <assert.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid_solver.h"
#include "./red_rec_utils/red_rec_utils.h"

static void solve_receiver(struct Grid *grid,
                           struct Reconfiguration *reconfiguration,
                           struct DelayedMoves *delayed_moves,
                           struct Range target_range,
                           struct ColumnPair column_pair,
                           const RedRecParallelParams *params) {
  int receiver_pivot = get_receiver_pivot(grid, delayed_moves, target_range,
                                          column_pair, params->linear_solver);

  struct Range *fixed_sources_range = &(struct Range){
      .start = receiver_pivot,
      .exclusive_end = receiver_pivot,
  };

  execute_move(grid, reconfiguration, fixed_sources_range,
               (struct ColumnPair){
                   .donor_index = column_pair.receiver_index,
                   .receiver_index = column_pair.receiver_index,
               });

  // Execute delayed moves and delete them
  int filtered_i = 0;
  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      execute_move(grid, reconfiguration, fixed_sources_range,
                   delayed_moves->array[i]);
    } else {
      delayed_moves->array[filtered_i] = delayed_moves->array[i];
      filtered_i++;
    }
  }
  delayed_moves->length = filtered_i;

  execute_move(grid, reconfiguration, fixed_sources_range, column_pair);
}

struct ThreadInputContext {
  struct Grid *grid;
  struct Reconfiguration *reconfiguration;
  struct Range target_range;
};

struct ThreadSyncVariables {
  pthread_barrier_t *barrier;
  pthread_mutex_t *mutex;
};

struct ThreadSharedVariables {
  struct Counts *column_counts;
  struct Counts *total_counts;
};

struct ThreadInput {
  struct ThreadInputContext context;
  struct ThreadSyncVariables sync;
  struct ThreadSharedVariables shared;
  const RedRecParallelParams *params;
  int thread_index;
};

static void *red_rec_parallel_thread(void *thread_input) {
  struct ThreadInput *input = (struct ThreadInput *)thread_input;

  struct Range column_range =
      get_range(input->thread_index, input->params->thread_num,
                input->context.grid->width);

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    input->shared.column_counts[i] = interval_get_counts(&(struct Interval){
        .array = grid_get_column(input->context.grid, i),
        .length = input->context.grid->height,
    });
  }

  for (int i = column_range.start; i < column_range.exclusive_end; i++) {
    if (counts_get_imbalance(input->shared.column_counts[i]) >= 0) {
      struct Mapping *mapping = input->params->linear_solver->solve(
          &(const struct Interval){
              .array = grid_get_column(input->context.grid, i),
              .length = input->context.grid->height,
          },
          input->params->linear_solver->params);

      pthread_mutex_lock(input->sync.mutex);
      reconfiguration_add_mapping(input->context.reconfiguration,
                                  input->context.grid, mapping, i);
      pthread_mutex_unlock(input->sync.mutex);

      mapping_free(mapping);
    }
  }

  pthread_exit(0);
}

struct Reconfiguration *red_rec_parallel(const struct Grid *grid,
                                         const void *params) {
  assert(grid_target_region_is_compact(grid));
  assert(params != NULL);

  int thread_num = ((RedRecParallelParams *)params)->thread_num;
  assert(thread_num > 0);

  pthread_barrier_t *barrier = malloc(sizeof(pthread_barrier_t));
  pthread_barrier_init(barrier, NULL, thread_num);

  pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
  pthread_mutex_init(mutex, NULL);

  struct ThreadInput *thread_inputs =
      malloc(thread_num * sizeof(struct ThreadInput));
  pthread_t *thread_ids = malloc(thread_num * sizeof(pthread_t));

  struct Grid *grid_copy = grid_get_copy(grid);
  struct Range target_range = grid_get_compact_target_region_range(grid);
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * grid_copy->width * grid_copy->height);

  struct Counts *column_counts = malloc(grid->width * sizeof(struct Counts));
  struct Counts total_counts = {0, 0};

  for (int i = 0; i < thread_num; i++) {
    thread_inputs[i] = (struct ThreadInput){
        .context =
            {
                .reconfiguration = reconfiguration,
                .grid = grid_copy,
                .target_range = target_range,
            },
        .shared =
            {
                .column_counts = column_counts,
                .total_counts = &total_counts,
            },
        .sync =
            {
                .barrier = barrier,
                .mutex = mutex,
            },
        .params = params,
        .thread_index = i,
    };

    pthread_create(&thread_ids[i], NULL, red_rec_parallel_thread,
                   &thread_inputs[i]);
  }

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  pthread_barrier_destroy(barrier);
  pthread_mutex_destroy(mutex);
  free(mutex);
  free(barrier);
  free(thread_inputs);
  free(thread_ids);

  for (int i = 0; i < grid->width; i++) {
    total_counts.source_num += column_counts[i].source_num;
    total_counts.target_num += column_counts[i].target_num;
  }

  if (counts_get_imbalance(total_counts) < 0) {
    free(column_counts);
    reconfiguration_free(reconfiguration);
    grid_free(grid_copy);
    return NULL;
  }

  struct DelayedMoves *delayed_moves =
      delayed_moves_new(grid_copy->width * grid_copy->height);

  struct ColumnPair best_pair = column_pair_get_best(grid_copy, column_counts);
  while (column_pair_exists(best_pair)) {
    if (best_pair.exchanged_sources_num != best_pair.receiver_deficit) {
      delayed_moves_add(delayed_moves, best_pair);
    } else {
      solve_receiver(grid_copy, reconfiguration, delayed_moves, target_range,
                     best_pair, params);
    }

    column_counts[best_pair.donor_index].source_num -=
        best_pair.exchanged_sources_num;
    column_counts[best_pair.receiver_index].source_num +=
        best_pair.exchanged_sources_num;

    best_pair = column_pair_get_best(grid_copy, column_counts);
  }

  reconfiguration_filter_identical(reconfiguration);

  free(column_counts);
  delayed_moves_free(delayed_moves);
  grid_free(grid_copy);
  return reconfiguration;
}
