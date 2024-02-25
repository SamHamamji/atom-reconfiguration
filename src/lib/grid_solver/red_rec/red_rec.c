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

static void execute_delayed_move(struct Grid *grid,
                                 struct Reconfiguration *reconfiguration,
                                 struct ColumnPair column_pair,
                                 struct Range fixed_sources_range) {
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);

  for (int bool_value = 0; bool_value < 2; bool_value++) {
    bool moving_upper_sources = (bool)bool_value;
    int target_row = moving_upper_sources ? fixed_sources_range.start - 1
                                          : fixed_sources_range.exclusive_end;
    for (int i = target_row; 0 <= i && i < grid->height;
         i += moving_upper_sources ? -1 : 1) {
      if (!donor[target_row].is_target) {
        break;
      }

      if (donor[i].is_source && !donor[i].is_target) {
        struct Move horizontal_move = {
            .origin = {.col = column_pair.donor_index, .row = i},
            .destination = {.col = column_pair.receiver_index, .row = i},
        };
        reconfiguration_add_move(reconfiguration, horizontal_move);
        grid_apply_move(grid, horizontal_move);

        struct Move vertical_move = {
            .origin = {.col = column_pair.receiver_index, .row = i},
            .destination = {.col = column_pair.receiver_index,
                            .row = target_row},
        };
        reconfiguration_add_move(reconfiguration, vertical_move);
        grid_apply_move(grid, vertical_move);

        target_row += moving_upper_sources ? -1 : 1;
      }
    }
  }
}

static void reconfigure_receiver_sources(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    int receiver_index, struct Range target_region_range,
    const RedRecParams *params) {
  struct Point *receiver = grid_get_column(grid, receiver_index);
  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));
  for (int i = 0; i < grid->height; i++) {
    receiver_alias[i] = (struct Point){
        .is_source = receiver[i].is_source,
        .is_target = target_region_range.start <= i &&
                     i < target_region_range.exclusive_end &&
                     receiver[i].is_target,
    };
  }

  struct Mapping *mapping = params->linear_solver->solve(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      params->linear_solver->params);
  reconfiguration_add_mapping(reconfiguration, grid, mapping, receiver_index);
  mapping_free(mapping);
  free(receiver_alias);
}

static struct Range get_receiver_sources_range(
    const struct Grid *grid, const struct Range target_range,
    const struct DelayedMoves *delayed_moves, struct ColumnPair column_pair,
    const RedRecParams *params) {
  struct SourceLocations delayed_moves_sources =
      get_delayed_moves_source_locations(grid, delayed_moves,
                                         column_pair.receiver_index);

  struct SourceLocations receiver_sources =
      get_column_source_locations(grid, column_pair.receiver_index);

  struct SourceLocations total_sources = receiver_sources;
  total_sources.upper_reservoir += delayed_moves_sources.upper_reservoir;
  total_sources.lower_reservoir += delayed_moves_sources.lower_reservoir;

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

  for (int i = target_range.start; i < target_range.exclusive_end; i++) {
    if (total_sources.upper_reservoir == 0) {
      break;
    }
    if (!receiver_alias[i].is_source) {
      receiver_alias[i].is_source = true;
      total_sources.target_region++;
      total_sources.upper_reservoir--;
    }
  }

  for (int i = target_range.exclusive_end - 1; i >= target_range.start; i--) {
    if (total_sources.lower_reservoir == 0) {
      break;
    }
    if (!receiver_alias[i].is_source) {
      receiver_alias[i].is_source = true;
      total_sources.target_region++;
      total_sources.lower_reservoir--;
    }
  }

  assert(total_sources.upper_reservoir == 0);
  assert(total_sources.lower_reservoir == 0);

  struct Mapping *mapping = params->linear_solver->solve(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      params->linear_solver->params);

  free(receiver_alias);

  for (int i = 0; i < mapping->pair_count; i++) {
    if (mapping->pairs[i].source >= mapping->pairs[0].target) {
      struct Range temp;
      temp.start =
          mapping->pairs[i].target + delayed_moves_sources.upper_reservoir;
      temp.exclusive_end = temp.start + receiver_sources.upper_reservoir +
                           receiver_sources.target_region +
                           receiver_sources.lower_reservoir;

      mapping_free(mapping);
      return temp;
    }
  }

  struct Range temp;
  temp.start = mapping->pairs[mapping->pair_count - 1].target + 1;
  temp.exclusive_end = temp.start;

  mapping_free(mapping);
  return temp;
}

static void solve_receiver(struct Grid *grid, struct Range target_range,
                           struct Reconfiguration *reconfiguration,
                           struct DelayedMoves *delayed_moves,
                           struct ColumnPair column_pair,
                           const RedRecParams *params) {
  struct Range sources_range = get_receiver_sources_range(
      grid, target_range, delayed_moves, column_pair, params);

  reconfigure_receiver_sources(
      grid, reconfiguration, column_pair.receiver_index, sources_range, params);

  // Solve delayed moves
  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      struct SourceLocations source_locations = get_column_source_locations(
          grid, delayed_moves->array[i].donor_index);
      execute_delayed_move(grid, reconfiguration, delayed_moves->array[i],
                           sources_range);
      sources_range.start -= source_locations.upper_reservoir;
      sources_range.exclusive_end += source_locations.lower_reservoir;
    }
  }

  execute_delayed_move(grid, reconfiguration, column_pair, sources_range);
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
      solve_receiver(copy, target_range, reconfiguration, delayed_moves,
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
