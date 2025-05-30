#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid_solver.h"
#include "./red_rec_utils/red_rec_utils.h"

static void solve_donors(struct Grid *grid,
                         struct Reconfiguration *reconfiguration,
                         const struct Counts *column_counts,
                         const RedRecParams *params) {
  for (int column_index = 0; column_index < grid->width; column_index++) {
    if (counts_get_imbalance(column_counts[column_index]) >= 0) {
      solve_donor(grid, reconfiguration, column_index, params->linear_solver);
    }
  }
}

static void solve_receiver(struct Grid *grid,
                           struct Reconfiguration *reconfiguration,
                           struct ReceiverDelayedMoves delayed_moves,
                           struct Range target_range,
                           const RedRecParams *params) {
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

static void solve_receivers_alternated(struct Grid *grid,
                                       struct Reconfiguration *reconfiguration,
                                       struct Counts *column_counts,
                                       struct Range target_range,
                                       const RedRecParams *params) {
  struct DelayedMoves delayed_moves = delayed_moves_new(grid);
  struct ColumnPairPQ column_pair_pq =
      column_pair_pq_new(column_counts, grid->width, params->pq_type);

  while (!column_pair_pq_is_empty(&column_pair_pq)) {
    struct ColumnPair best_pair = column_pair_pq_pop(&column_pair_pq);
    assert(get_exchange_num(best_pair) > 0);

    delayed_moves_add(delayed_moves, best_pair);

    if (get_exchange_num(best_pair) == -best_pair.receiver_deficit) {
      solve_receiver(grid, reconfiguration,
                     delayed_moves.array[best_pair.receiver_index],
                     target_range, params);
    }
  }

  column_pair_pq_free(&column_pair_pq);
  delayed_moves_free(delayed_moves);
}

static void solve_receivers_deferred(struct Grid *grid,
                                     struct Reconfiguration *reconfiguration,
                                     struct Counts *column_counts,
                                     struct Range target_range,
                                     const RedRecParams *params) {
  struct DelayedMoves delayed_moves = delayed_moves_new(grid);
  struct ColumnPairPQ column_pair_pq =
      column_pair_pq_new(column_counts, grid->width, params->pq_type);
  struct ReceiverOrder *receiver_order = receiver_order_new(grid->width);

  while (!column_pair_pq_is_empty(&column_pair_pq)) {
    struct ColumnPair best_pair = column_pair_pq_pop(&column_pair_pq);
    assert(get_exchange_num(best_pair) > 0);

    delayed_moves_add(delayed_moves, best_pair);
    if (get_exchange_num(best_pair) == -best_pair.receiver_deficit) {
      receiver_order_push(receiver_order, best_pair.receiver_index);
    }
  }

  column_pair_pq_free(&column_pair_pq);

  for (int i = 0; i < receiver_order->receiver_num; i++) {
    solve_receiver(grid, reconfiguration,
                   delayed_moves.array[receiver_order->receiver_indexes[i]],
                   target_range, params);
  }

  delayed_moves_free(delayed_moves);
  receiver_order_free(receiver_order);
}

struct Reconfiguration *red_rec(struct Grid *grid, const void *params) {
  assert(grid_target_region_is_compact(grid));
  assert(params != NULL);

  RedRecParams *red_rec_params = (RedRecParams *)params;

  struct Counts *column_counts = grid_get_column_counts(grid);

  struct Counts total_counts = {0, 0};
  for (int i = 0; i < grid->width; i++) {
    total_counts.source_num += column_counts[i].source_num;
    total_counts.target_num += column_counts[i].target_num;
  }

  if (counts_get_imbalance(total_counts) < 0) {
    free(column_counts);
    return NULL;
  }

  struct Range target_range = grid_get_compact_target_region_range(grid);
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * total_counts.target_num);

  solve_donors(grid, reconfiguration, column_counts, red_rec_params);

  if (red_rec_params->receiver_solving_order == ALTERNATED_SOLVING) {
    solve_receivers_alternated(grid, reconfiguration, column_counts,
                               target_range, red_rec_params);
  } else {
    solve_receivers_deferred(grid, reconfiguration, column_counts, target_range,
                             red_rec_params);
  }

  assert(grid_is_solved(grid));
  free(column_counts);

  reconfiguration_filter_identical(reconfiguration);

  return reconfiguration;
}
