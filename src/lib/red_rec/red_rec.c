#include <assert.h>
#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid/grid.h"
#include "../linear_solver/linear_solver.h"
#include "../utils/max_min.h"
#include "./red_rec.h"
#include "delayed_moves.h"

struct SourceLocations {
  int upper_reservoir;
  int target_region;
  int lower_reservoir;
};

static struct SourceLocations get_column_source_locations(struct Grid *grid,
                                                          int column_index) {
  struct SourceLocations column_source_locations = {0, 0, 0};
  struct Point *column = grid_get_column(grid, column_index);

  int i = 0;
  while (i < grid->height && !column[i].is_target) {
    column_source_locations.upper_reservoir += (int)column[i].is_source;
    i++;
  }
  while (i < grid->height && column[i].is_target) {
    column_source_locations.target_region += (int)column[i].is_source;
    i++;
  }
  while (i < grid->height && !column[i].is_target) {
    column_source_locations.lower_reservoir += (int)column[i].is_source;
    i++;
  }
  return column_source_locations;
}

static void
solve_self_sufficient_columns(struct Grid *grid, struct Counts *column_counts,
                              struct Reconfiguration *reconfiguration,
                              bool *column_is_solved) {
  for (int column_index = 0; column_index < grid->width; column_index++) {
    if (counts_get_imbalance(column_counts[column_index]) >= 0) {
      struct Point *column = grid_get_column(grid, column_index);
      struct Mapping *mapping = linear_solve_aggarwal(
          &(const struct Interval){.array = column, .length = grid->height},
          NULL);

      reconfiguration_add_mapping(reconfiguration, grid, mapping, column_index);

      mapping_free(mapping);
      column_is_solved[column_index] = true;
    }
  }
}

static void redistribute_and_reconfigure(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    struct ColumnPair column_pair, struct Range target_region_range) {
  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);

  for (int i = 0; i < grid->height; i++) {
    receiver_alias[i] = (struct Point){
        .is_source = receiver[i].is_source ||
                     (donor[i].is_source && !donor[i].is_target),
        .is_target = target_region_range.start <= i &&
                     i < target_region_range.exclusive_end &&
                     receiver[i].is_target,
    };
  }

  struct Mapping *mapping = linear_solve_aggarwal(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      NULL);
  for (int i = 0; i < mapping->pair_count; i++) {
    if (mapping->pairs[i].source != mapping->pairs[i].target) {
      struct Move move = (struct Move){
          .origin = {.col = column_pair.donor_index,
                     .row = mapping->pairs[i].source},
          .destination = {.col = column_pair.receiver_index,
                          .row = mapping->pairs[i].source},
      };
      reconfiguration_add_move(reconfiguration, move);
      grid_apply_move(grid, move);
    }
  }

  reconfiguration_add_mapping(reconfiguration, grid, mapping,
                              column_pair.receiver_index);
  mapping_free(mapping);
  free(receiver_alias);
}

static struct SourceLocations
get_delayed_moves_source_locations(struct Grid *grid,
                                   struct DelayedMoves *delayed_moves,
                                   struct ColumnPair column_pair) {
  struct SourceLocations source_locations = {0, 0, 0};

  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      struct SourceLocations donor_source_locations =
          get_column_source_locations(grid,
                                      delayed_moves->array[i].donor_index);
      source_locations.upper_reservoir +=
          donor_source_locations.upper_reservoir;
      source_locations.lower_reservoir +=
          donor_source_locations.lower_reservoir;
    }
  }

  return source_locations;
}

static void delayed_moves_solve(struct Grid *grid,
                                struct Reconfiguration *reconfiguration,
                                struct DelayedMoves *delayed_moves,
                                struct ColumnPair column_pair) {
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);
  struct Counts receiver_counts = interval_get_counts(
      &(struct Interval){.array = receiver, .length = grid->height});

  struct SourceLocations delayed_moves_sources =
      get_delayed_moves_source_locations(grid, delayed_moves, column_pair);

  struct SourceLocations last_donor_sources =
      get_column_source_locations(grid, column_pair.donor_index);

  struct Range target_range = {
      .start = INT_MAX,
      .exclusive_end = INT_MIN,
  };

  for (int i = 0; i < grid->height; i++) {
    if (receiver[i].is_target) {
      target_range.start =
          i + delayed_moves_sources.upper_reservoir +
          min(last_donor_sources.upper_reservoir,
              receiver_counts.target_num - receiver_counts.source_num -
                  delayed_moves_sources.upper_reservoir -
                  delayed_moves_sources.lower_reservoir);
      target_range.exclusive_end =
          target_range.start + receiver_counts.source_num;
      break;
    }
  }

  redistribute_and_reconfigure(
      grid, reconfiguration,
      (struct ColumnPair){.donor_index = column_pair.receiver_index,
                          .receiver_index = column_pair.receiver_index},
      target_range);

  // Solve delayed movements
  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      struct SourceLocations source_locations = get_column_source_locations(
          grid, delayed_moves->array[i].donor_index);
      target_range.start -= source_locations.upper_reservoir;
      target_range.exclusive_end += source_locations.lower_reservoir;
      redistribute_and_reconfigure(grid, reconfiguration,
                                   delayed_moves->array[i], target_range);
    }
  }

  target_range.start -= last_donor_sources.upper_reservoir;
  target_range.exclusive_end += last_donor_sources.lower_reservoir;

  redistribute_and_reconfigure(grid, reconfiguration, column_pair,
                               target_range);
}

struct Reconfiguration *red_rec(const struct Grid *grid) {
  assert(grid_target_region_is_compact(grid));
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
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * copy->width * copy->height);
  bool *column_is_solved = calloc(copy->width, sizeof(bool));

  solve_self_sufficient_columns(copy, column_counts, reconfiguration,
                                column_is_solved);

  struct DelayedMoves *delayed_moves =
      delayed_moves_new(copy->width * copy->height);

  struct ColumnPair best_pair =
      column_pair_get_best(copy, column_counts, column_is_solved);
  while (best_pair.receiver_index != -1 && best_pair.donor_index != -1) {

    int receiver_deficit =
        abs(counts_get_imbalance(column_counts[best_pair.receiver_index]));
    int donor_imbalance =
        counts_get_imbalance(column_counts[best_pair.donor_index]);

    if (donor_imbalance < receiver_deficit) {
      delayed_moves_add(delayed_moves, best_pair);
    } else {
      delayed_moves_solve(copy, reconfiguration, delayed_moves, best_pair);
      column_is_solved[best_pair.receiver_index] = true;
    }

    int redistributed_sources = min(donor_imbalance, receiver_deficit);
    column_counts[best_pair.donor_index].source_num -= redistributed_sources;
    column_counts[best_pair.receiver_index].source_num += redistributed_sources;

    best_pair = column_pair_get_best(copy, column_counts, column_is_solved);
  }

  reconfiguration_filter_identical(reconfiguration);

  free(column_counts);
  free(column_is_solved);
  delayed_moves_free(delayed_moves);
  grid_free(copy);
  return reconfiguration;
}
