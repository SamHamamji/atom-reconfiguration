#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "../grid_solver.h"
#include "./red_rec_parallel_utils/red_rec_parallel_utils.h"
#include "./red_rec_utils/red_rec_utils.h"

struct ThreadSyncVariables {
  pthread_barrier_t *barrier;
  pthread_mutex_t *reconfiguration_mutex;
  pthread_mutex_t *delayed_moves_mutex;
  int *delayed_moves_counter;
  pthread_mutex_t *column_mutexes;
  sem_t *delayed_moves_semaphore;
  sem_t *column_counts_semaphore;
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

static void execute_last_move(struct Grid *grid,
                              struct Reconfiguration *reconfiguration,
                              const struct Mapping *last_donor_mapping,
                              int last_donor_index, int receiver_index) {
  struct Point *last_donor = grid_get_column(grid, last_donor_index);
  struct Point *receiver = grid_get_column(grid, receiver_index);

  for (int i = 0; i < last_donor_mapping->pair_count; i++) {
    struct Pair pair = last_donor_mapping->pairs[i];
    if (last_donor[pair.source].is_target) {
      continue;
    }

    reconfiguration_add_move(
        reconfiguration,
        (struct Move){
            .origin = {.col = last_donor_index, .row = pair.source},
            .destination = {.col = receiver_index, .row = pair.source},
        });

    receiver[pair.target].is_source = true;
  }

  for (int i = 0; i < last_donor_mapping->pair_count; i++) {
    struct Pair pair = last_donor_mapping->pairs[i];
    if (last_donor[pair.source].is_target || pair.target > pair.source) {
      continue;
    }

    reconfiguration_add_move(
        reconfiguration,
        (struct Move){
            .origin = {.col = receiver_index, .row = pair.source},
            .destination = {.col = receiver_index, .row = pair.target},
        });
  }

  for (int i = last_donor_mapping->pair_count - 1; i >= 0; i--) {
    struct Pair pair = last_donor_mapping->pairs[i];
    if (last_donor[pair.source].is_target || pair.source > pair.target) {
      continue;
    }

    reconfiguration_add_move(
        reconfiguration,
        (struct Move){
            .origin = {.col = receiver_index, .row = pair.source},
            .destination = {.col = receiver_index, .row = pair.target},
        });
  }
}

static void extract_last_donor_sources(const struct Mapping *last_donor_mapping,
                                       struct Point *last_donor) {
  for (int i = 0; i < last_donor_mapping->pair_count; i++) {
    struct Pair pair = last_donor_mapping->pairs[i];
    if (last_donor[pair.source].is_target) {
      continue;
    }

    last_donor[pair.source].is_source = false;
  }
}

static struct ThreadSyncVariables thread_sync_variables_new(int thread_num,
                                                            int grid_width) {
  struct ThreadSyncVariables sync = {
      .barrier = malloc(sizeof(pthread_barrier_t)),
      .reconfiguration_mutex = malloc(sizeof(pthread_mutex_t)),
      .delayed_moves_mutex = malloc(sizeof(pthread_mutex_t)),
      .delayed_moves_counter = malloc(sizeof(int)),
      .column_mutexes = malloc(grid_width * sizeof(pthread_mutex_t)),
      .delayed_moves_semaphore = malloc(sizeof(sem_t)),
      .column_counts_semaphore = malloc(sizeof(sem_t)),
  };
  pthread_barrier_init(sync.barrier, NULL, thread_num);
  pthread_mutex_init(sync.reconfiguration_mutex, NULL);
  pthread_mutex_init(sync.delayed_moves_mutex, NULL);
  *sync.delayed_moves_counter = 0;
  for (int i = 0; i < grid_width; i++) {
    pthread_mutex_init(&sync.column_mutexes[i], NULL);
  }
  sem_init(sync.delayed_moves_semaphore, 0, 0);
  sem_init(sync.column_counts_semaphore, 0, 0);
  return sync;
}

static void thread_sync_variables_free(struct ThreadSyncVariables sync,
                                       int grid_width) {
  pthread_barrier_destroy(sync.barrier);
  pthread_mutex_destroy(sync.reconfiguration_mutex);
  pthread_mutex_destroy(sync.delayed_moves_mutex);
  for (int i = 0; i < grid_width; i++) {
    pthread_mutex_destroy(&sync.column_mutexes[i]);
  }
  sem_destroy(sync.delayed_moves_semaphore);
  sem_destroy(sync.column_counts_semaphore);
  free(sync.barrier);
  free(sync.reconfiguration_mutex);
  free(sync.delayed_moves_mutex);
  free(sync.delayed_moves_counter);
  free(sync.column_mutexes);
  free(sync.delayed_moves_semaphore);
  free(sync.column_counts_semaphore);
}

static void thread_shared_variables_free(struct ThreadSharedVariables shared) {
  free(shared.column_counts);
  free(shared.total_counts);
  delayed_moves_free(shared.delayed_moves);
  receiver_order_free(shared.receiver_order);
}

static struct Reconfiguration *
generate_receiver_reconfiguration(struct ThreadInput *input,
                                  int receiver_index) {
  struct ReceiverDelayedMoves receiver_delayed_moves =
      input->shared.delayed_moves.array[receiver_index];

  struct Mapping *last_donor_mapping = get_last_donor_mapping(
      input->context.grid, receiver_delayed_moves, input->context.target_range,
      input->params->linear_solver);

  int last_donor_index =
      receiver_delayed_moves.pairs[receiver_delayed_moves.length - 1]
          .donor_index;

  extract_last_donor_sources(
      last_donor_mapping,
      grid_get_column(input->context.grid, last_donor_index));

  pthread_mutex_unlock(&input->sync.column_mutexes[last_donor_index]);

  int receiver_pivot =
      get_pivot_from_mapping(last_donor_mapping, input->context.target_range);

  struct Range *fixed_sources_range = &(struct Range){
      .start = receiver_pivot,
      .exclusive_end = receiver_pivot,
  };

  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * (input->context.target_range.exclusive_end -
                               input->context.target_range.start));

  execute_move(input->context.grid, reconfiguration, fixed_sources_range,
               (struct ColumnPair){
                   .donor_index = receiver_index,
                   .receiver_index = receiver_index,
               });

  for (int i = 0; i < receiver_delayed_moves.length - 1; i++) {
    execute_move(input->context.grid, reconfiguration, fixed_sources_range,
                 receiver_delayed_moves.pairs[i]);
  }

  execute_last_move(input->context.grid, reconfiguration, last_donor_mapping,
                    last_donor_index, receiver_index);

  mapping_free(last_donor_mapping);

  return reconfiguration;
}

static void execute_delayed_moves(struct ThreadInput *input) {

  while (true) {
    sem_wait(input->sync.delayed_moves_semaphore);

    pthread_mutex_lock(input->sync.delayed_moves_mutex);
    int i = (*input->sync.delayed_moves_counter)++;

    if (i >= input->shared.receiver_order->receiver_num) {
      pthread_mutex_unlock(input->sync.delayed_moves_mutex);
      break;
    }

    int receiver_index = input->shared.receiver_order->receiver_indexes[i];
    struct ReceiverDelayedMoves delayed_moves =
        input->shared.delayed_moves.array[receiver_index];

    int last_donor_index =
        delayed_moves.pairs[delayed_moves.length - 1].donor_index;
    pthread_mutex_lock(&input->sync.column_mutexes[last_donor_index]);

    pthread_mutex_unlock(input->sync.delayed_moves_mutex);

    for (int j = 0; j < delayed_moves.length - 1; j++) {
      // Donor will be emptied and never used again
      pthread_mutex_lock(
          &input->sync.column_mutexes[delayed_moves.pairs[j].donor_index]);
    }

    // Releases the last donor mutex
    struct Reconfiguration *private_reconfiguration =
        generate_receiver_reconfiguration(input, receiver_index);

    assert(private_reconfiguration->move_count ==
           2 * (input->context.target_range.exclusive_end -
                input->context.target_range.start));

    int copy_destination = input->context.reconfiguration->move_count +
                           i * private_reconfiguration->move_count;

    memcpy(&input->context.reconfiguration->moves[copy_destination],
           private_reconfiguration->moves,
           private_reconfiguration->move_count *
               sizeof(private_reconfiguration->moves[0]));

    reconfiguration_free(private_reconfiguration);
  }
}

static void *red_rec_parallel_thread(void *thread_input) {
  struct ThreadInput *input = (struct ThreadInput *)thread_input;

  struct Range column_range =
      get_range(input->thread_index, input->params->thread_num,
                input->context.grid->width);

  struct Counts *private_counts =
      malloc((column_range.exclusive_end - column_range.start) *
             sizeof(struct Counts));

  compute_counts_parallel(input->context.grid, private_counts,
                          input->shared.total_counts,
                          input->sync.reconfiguration_mutex, column_range);

  memcpy(&input->shared.column_counts[column_range.start], private_counts,
         (column_range.exclusive_end - column_range.start) *
             sizeof(struct Counts));

  sem_post(input->sync.column_counts_semaphore);

  solve_donors_parallel(input->context, private_counts,
                        input->params->linear_solver,
                        input->sync.reconfiguration_mutex, column_range);

  free(private_counts);

  pthread_barrier_wait(input->sync.barrier);

  if (counts_get_imbalance(*input->shared.total_counts) >= 0) {
    execute_delayed_moves(input);
  }

  pthread_exit(NULL);
}

static void produce_delayed_moves(struct Grid *grid,
                                  struct ThreadSharedVariables shared,
                                  sem_t *delayed_moves_semaphore,
                                  const RedRecParallelParams *params) {
  struct ColumnPairPQ column_pair_pq =
      column_pair_pq_new(shared.column_counts, grid->width, params->pq_type);

  while (!column_pair_pq_is_empty(&column_pair_pq)) {
    struct ColumnPair best_pair = column_pair_pq_pop(&column_pair_pq);
    delayed_moves_add(shared.delayed_moves, best_pair);

    if (get_exchange_num(best_pair) == -best_pair.receiver_deficit) {
      receiver_order_push(shared.receiver_order, best_pair.receiver_index);
      sem_post(delayed_moves_semaphore);
    }
  }

  column_pair_pq_free(&column_pair_pq);

  for (int i = 0; i < params->thread_num; i++) {
    sem_post(delayed_moves_semaphore);
  }
}

struct Reconfiguration *
red_rec_parallel_multiple_consumers(struct Grid *grid, const void *params) {
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
  struct ThreadSyncVariables sync =
      thread_sync_variables_new(thread_num, grid->width);

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
    sem_wait(sync.column_counts_semaphore);
  }

  if (counts_get_imbalance(*shared.total_counts) < 0) {
    for (int i = 0; i < thread_num; i++) {
      pthread_join(thread_ids[i], NULL);
    }

    reconfiguration_free(context.reconfiguration);
    thread_sync_variables_free(sync, grid->width);
    thread_shared_variables_free(shared);
    free(thread_inputs);
    free(thread_ids);

    return NULL;
  }

  produce_delayed_moves(grid, shared, sync.delayed_moves_semaphore, params);

  for (int i = 0; i < thread_num; i++) {
    pthread_join(thread_ids[i], NULL);
  }

  // Increment move count by two moves per target for every receiver column
  context.reconfiguration->move_count +=
      2 * shared.receiver_order->receiver_num *
      (context.target_range.exclusive_end - context.target_range.start);

  thread_sync_variables_free(sync, grid->width);
  thread_shared_variables_free(shared);
  free(thread_inputs);
  free(thread_ids);

  reconfiguration_filter_identical(context.reconfiguration);

  return context.reconfiguration;
}
