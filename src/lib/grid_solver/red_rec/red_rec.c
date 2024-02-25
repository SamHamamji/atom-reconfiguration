#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../../grid/grid.h"
#include "../../linear_solver/linear_solver.h"
#include "../common/delayed_moves.h"
#include "../common/source_locations.h"
#include "../grid_solver.h"

static void solve_self_sufficient_columns(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    const struct Counts *column_counts, const RedRecParams *params) {
  for (int column_index = 0; column_index < grid->width; column_index++) {
    if (counts_get_imbalance(column_counts[column_index]) >= 0) {
      struct Mapping *mapping = params->linear_solver->solve(
          &(const struct Interval){
              .array = grid_get_column(grid, column_index),
              .length = grid->height,
          },
          params->linear_solver->params);

      reconfiguration_add_mapping(reconfiguration, grid, mapping, column_index);

      mapping_free(mapping);
    }
  }
}

/**
 * Returns a column representing the receiver with its sources and the sources
 * from its delayed donors squished into its target region. The last donor's
 * sources are simply moved to its target region.
 * It is meant to decide know which sources from the last donor are meant to be
 * used while minimizing the vertically moved distance.
 */
static struct Point *get_receiver_squished_sources_alias(
    const struct Grid *grid, const struct DelayedMoves *delayed_moves,
    struct Range target_range, struct ColumnPair column_pair) {
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);

  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));
  for (int i = 0; i < grid->height; i++) {
    receiver_alias[i] = (struct Point){
        .is_source = (donor[i].is_source && !donor[i].is_target) ||
                     (receiver[i].is_source && receiver[i].is_target),
        .is_target = receiver[i].is_target,
    };
  }

  struct SourceLocations receiver_sources =
      get_column_source_locations(grid, column_pair.receiver_index);

  struct SourceLocations total_sources = get_delayed_moves_source_locations(
      grid, delayed_moves, column_pair.receiver_index);
  total_sources.upper_reservoir += receiver_sources.upper_reservoir;
  total_sources.lower_reservoir += receiver_sources.lower_reservoir;

  struct Range squished_sources_range = target_range;
  for (int i = 0; i < total_sources.upper_reservoir; i++) {
    while (receiver_alias[squished_sources_range.start].is_source) {
      squished_sources_range.start++;
    }
    receiver_alias[squished_sources_range.start].is_source = true;
    squished_sources_range.start++;
  }

  for (int i = 0; i < total_sources.lower_reservoir; i++) {
    while (receiver_alias[squished_sources_range.exclusive_end - 1].is_source) {
      squished_sources_range.exclusive_end--;
    }
    receiver_alias[squished_sources_range.exclusive_end - 1].is_source = true;
    squished_sources_range.exclusive_end--;
  }

  assert(squished_sources_range.start < squished_sources_range.exclusive_end);

  return receiver_alias;
}

static int get_receiver_pivot(const struct Grid *grid,
                              const struct DelayedMoves *delayed_moves,
                              struct Range target_range,
                              struct ColumnPair column_pair,
                              const RedRecParams *params) {
  struct Point *receiver_alias = get_receiver_squished_sources_alias(
      grid, delayed_moves, target_range, column_pair);

  struct Mapping *mapping = params->linear_solver->solve(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      params->linear_solver->params);

  int pivot = target_range.exclusive_end;
  for (int i = 0; i < mapping->pair_count; i++) {
    // If source is moved up
    if (mapping->pairs[i].source > mapping->pairs[i].target ||
        (i > 0 && mapping->pairs[i].source == mapping->pairs[i].target &&
         mapping->pairs[i - 1].source < mapping->pairs[i - 1].target)) {
      pivot = mapping->pairs[i].target;
      break;
    }
  }

  free(receiver_alias);
  mapping_free(mapping);
  return pivot;
}

static void execute_move(struct Grid *grid,
                         struct Reconfiguration *reconfiguration,
                         struct Range *fixed_sources_range,
                         struct ColumnPair column_pair) {
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);

  for (int bool_value = 0; bool_value < 2; bool_value++) {
    bool moving_upper_sources = (bool)bool_value;
    int target_row = moving_upper_sources ? fixed_sources_range->start - 1
                                          : fixed_sources_range->exclusive_end;

    for (int i = target_row; 0 <= i && i < grid->height;
         i += moving_upper_sources ? -1 : 1) {
      if (!donor[target_row].is_target) {
        break;
      }

      if (!donor[i].is_source ||
          (donor[i].is_target &&
           column_pair.donor_index != column_pair.receiver_index)) {
        continue;
      }

      struct Move horizontal_move = {
          .origin = {.col = column_pair.donor_index, .row = i},
          .destination = {.col = column_pair.receiver_index, .row = i},
      };
      struct Move vertical_move = {
          .origin = {.col = column_pair.receiver_index, .row = i},
          .destination = {.col = column_pair.receiver_index, .row = target_row},
      };

      reconfiguration_add_move(reconfiguration, horizontal_move);
      reconfiguration_add_move(reconfiguration, vertical_move);

      grid_apply_move(grid, horizontal_move);
      grid_apply_move(grid, vertical_move);

      if (moving_upper_sources) {
        fixed_sources_range->start--;
        target_row--;
      } else {
        fixed_sources_range->exclusive_end++;
        target_row++;
      }
    }
  }
}

static void
solve_receiver(struct Grid *grid, struct Reconfiguration *reconfiguration,
               struct DelayedMoves *delayed_moves, struct Range target_range,
               struct ColumnPair column_pair, const RedRecParams *params) {
  int receiver_pivot = get_receiver_pivot(grid, delayed_moves, target_range,
                                          column_pair, params);

  struct Range *fixed_sources_range = &(struct Range){
      .start = receiver_pivot,
      .exclusive_end = receiver_pivot,
  };

  execute_move(grid, reconfiguration, fixed_sources_range,
               (struct ColumnPair){
                   .donor_index = column_pair.receiver_index,
                   .receiver_index = column_pair.receiver_index,
               });

  // Solve delayed moves
  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      execute_move(grid, reconfiguration, fixed_sources_range,
                   delayed_moves->array[i]);
    }
  }

  execute_move(grid, reconfiguration, fixed_sources_range, column_pair);
}

struct Reconfiguration *red_rec(const struct Grid *grid, const void *params) {
  assert(grid_target_region_is_compact(grid));
  assert(params != NULL);

  struct RedRecParams *red_rec_params = (struct RedRecParams *)params;

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

  struct Grid *copy = grid_get_copy(grid);
  struct Range target_range = grid_get_compact_target_region_range(grid);
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * copy->width * copy->height);

  solve_self_sufficient_columns(copy, reconfiguration, column_counts,
                                red_rec_params);

  struct DelayedMoves *delayed_moves =
      delayed_moves_new(copy->width * copy->height);

  struct ColumnPair best_pair = column_pair_get_best(copy, column_counts);
  while (column_pair_exists(best_pair)) {
    if (best_pair.exchanged_sources_num != best_pair.receiver_deficit) {
      delayed_moves_add(delayed_moves, best_pair);
    } else {
      solve_receiver(copy, reconfiguration, delayed_moves, target_range,
                     best_pair, params);
    }

    column_counts[best_pair.donor_index].source_num -=
        best_pair.exchanged_sources_num;
    column_counts[best_pair.receiver_index].source_num +=
        best_pair.exchanged_sources_num;

    best_pair = column_pair_get_best(copy, column_counts);
  }

  reconfiguration_filter_identical(reconfiguration);

  free(column_counts);
  delayed_moves_free(delayed_moves);
  grid_free(copy);
  return reconfiguration;
}
