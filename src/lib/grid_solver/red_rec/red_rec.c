#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid_solver.h"
#include "./red_rec_utils/red_rec_utils.h"

static void solve_self_sufficient_columns(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    const struct Counts *column_counts, const RedRecParams *params) {
  for (int column_index = 0; column_index < grid->width; column_index++) {
    if (counts_get_imbalance(column_counts[column_index]) >= 0) {
      solve_self_sufficient_column(grid, reconfiguration, column_index,
                                   params->linear_solver);
    }
  }
}

static void
solve_receiver(struct Grid *grid, struct Reconfiguration *reconfiguration,
               struct DelayedMoves *delayed_moves, struct Range target_range,
               struct ColumnPair column_pair, const RedRecParams *params) {
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

struct Reconfiguration *red_rec(const struct Grid *grid, const void *params) {
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

  struct Grid *grid_copy = grid_get_copy(grid);
  struct Range target_range = grid_get_compact_target_region_range(grid);
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * grid_copy->width * grid_copy->height);

  solve_self_sufficient_columns(grid_copy, reconfiguration, column_counts,
                                red_rec_params);

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
