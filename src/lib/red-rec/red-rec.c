#include <limits.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid/grid.h"
#include "../linear_solver/linear_solver.h"
#include "../utils/max_min.h"
#include "./red-rec.h"
#include "column_pair.h"
#include "move.h"
#include "reconfiguration.h"

struct DelayedMoves {
  struct ColumnPair *array;
  int length;
};

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

static void redistribute_and_reconfigure_without_1d(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    struct ColumnPair column_pair, struct Range target_region_range) {
  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);

  int source_count = 0;
  for (int j = 0; j < grid->height; j++) {
    if (donor[j].is_source && !donor[j].is_target) {
      reconfiguration_add_move(
          reconfiguration,
          (struct Move){
              .origin = {.col = column_pair.donor_index, .row = j},
              .destination = {.col = column_pair.receiver_index, .row = j},
          });
      source_count++;
    }

    receiver_alias[j] = (struct Point){
        .is_source = receiver[j].is_source ||
                     (donor[j].is_source && !donor[j].is_target),
        .is_target = target_region_range.start <= j &&
                     j < target_region_range.exclusive_end &&
                     receiver[j].is_target,
    };
  }

  struct Mapping *mapping = linear_solve_aggarwal(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      NULL);
  for (int j = 0; j < mapping->pair_count; j++) {
    if (mapping->pairs[j].source != mapping->pairs[j].target) {
      struct Move move = (struct Move){
          .origin = {.col = column_pair.donor_index,
                     .row = mapping->pairs[j].source},
          .destination = {.col = column_pair.receiver_index,
                          .row = mapping->pairs[j].source},
      };
      reconfiguration_add_move(reconfiguration, move);
      reconfiguration_apply_last_moves(reconfiguration, grid, 1);
    }
  }

  reconfiguration_add_mapping(reconfiguration, grid, mapping,
                              column_pair.receiver_index);
  mapping_free(mapping);
  free(receiver_alias);
}

static void redistribute_and_reconfigure(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    struct ColumnPair column_pair, struct Range target_region_range) {
  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);

  for (int j = 0; j < grid->height; j++) {
    receiver_alias[j] = (struct Point){
        .is_source = receiver[j].is_source ||
                     (donor[j].is_source && !donor[j].is_target),
        .is_target = target_region_range.start <= j &&
                     j < target_region_range.exclusive_end &&
                     receiver[j].is_target,
    };
  }

  struct Mapping *mapping = linear_solve_aggarwal(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      NULL);
  for (int j = 0; j < mapping->pair_count; j++) {
    if (mapping->pairs[j].source != mapping->pairs[j].target) {
      struct Move move = (struct Move){
          .origin = {.col = column_pair.donor_index,
                     .row = mapping->pairs[j].source},
          .destination = {.col = column_pair.receiver_index,
                          .row = mapping->pairs[j].source},
      };
      reconfiguration_add_move(reconfiguration, move);
      reconfiguration_apply_last_moves(reconfiguration, grid, 1);
    }
  }

  reconfiguration_add_mapping(reconfiguration, grid, mapping,
                              column_pair.receiver_index);
  mapping_free(mapping);
  free(receiver_alias);
}

static void solve_delayed_movements(struct Grid *grid,
                                    struct Reconfiguration *reconfiguration,
                                    struct DelayedMoves *delayed_moves,
                                    struct ColumnPair column_pair) {
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);
  struct Counts receiver_counts = {0, 0};
  for (int i = 0; i < grid->height; i++) {
    receiver_counts.source_num += (int)receiver[i].is_source;
    receiver_counts.target_num += (int)receiver[i].is_target;
  }

  struct SourceLocations redistributed_sources_locations = {0, 0, 0};
  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      struct SourceLocations donor_source_locations =
          get_column_source_locations(grid,
                                      delayed_moves->array[i].donor_index);
      redistributed_sources_locations.upper_reservoir +=
          donor_source_locations.upper_reservoir;
      redistributed_sources_locations.lower_reservoir +=
          donor_source_locations.lower_reservoir;
    }
  }

  struct SourceLocations last_donor_source_locations =
      get_column_source_locations(grid, column_pair.donor_index);

  redistributed_sources_locations.upper_reservoir +=
      min(last_donor_source_locations.upper_reservoir,
          receiver_counts.target_num - receiver_counts.source_num -
              redistributed_sources_locations.upper_reservoir -
              redistributed_sources_locations.lower_reservoir);
  redistributed_sources_locations.lower_reservoir +=
      min(last_donor_source_locations.lower_reservoir,
          receiver_counts.target_num - receiver_counts.source_num -
              redistributed_sources_locations.upper_reservoir);

  struct Range target_region_range = {
      .start = INT_MAX,
      .exclusive_end = INT_MIN,
  };

  for (int i = 0; i < grid->height; i++) {
    if (receiver[i].is_target) {
      target_region_range.start =
          i + redistributed_sources_locations.upper_reservoir;
      target_region_range.exclusive_end =
          target_region_range.start + receiver_counts.source_num;
      break;
    }
  }

  redistribute_and_reconfigure(
      grid, reconfiguration,
      (struct ColumnPair){.donor_index = column_pair.receiver_index,
                          .receiver_index = column_pair.receiver_index},
      target_region_range);

  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      struct SourceLocations source_locations = get_column_source_locations(
          grid, delayed_moves->array[i].donor_index);
      target_region_range.start -= source_locations.upper_reservoir;
      target_region_range.exclusive_end += source_locations.lower_reservoir;
      redistribute_and_reconfigure(
          grid, reconfiguration, delayed_moves->array[i], target_region_range);
    }
  }

  target_region_range.start -= last_donor_source_locations.upper_reservoir;
  target_region_range.exclusive_end +=
      last_donor_source_locations.lower_reservoir;

  redistribute_and_reconfigure(grid, reconfiguration, column_pair,
                               target_region_range);
}

static void solve_column_pair(struct Grid *grid,
                              struct Reconfiguration *reconfiguration,
                              struct DelayedMoves *delayed_moves,
                              struct Counts *column_counts,
                              bool *column_is_solved,
                              struct ColumnPair column_pair) {

  int redistributed_atoms_count =
      min(counts_get_imbalance(column_counts[column_pair.donor_index]),
          abs(counts_get_imbalance(column_counts[column_pair.receiver_index])));
  column_counts[column_pair.donor_index].source_num -=
      redistributed_atoms_count;
  column_counts[column_pair.receiver_index].source_num +=
      redistributed_atoms_count;

  if (counts_get_imbalance(column_counts[column_pair.receiver_index]) != 0) {
    delayed_moves->array[delayed_moves->length] = column_pair;
    delayed_moves->length++;
  } else {
    solve_delayed_movements(grid, reconfiguration, delayed_moves, column_pair);
    column_is_solved[column_pair.receiver_index] = true;
  }

  for (int i = 0; i < grid->width; i++) {
  }
}

struct Reconfiguration *red_rec(const struct Grid *grid) {
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
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * grid_copy->width * grid_copy->height);

  bool *column_is_solved = calloc(grid_copy->width, sizeof(bool));

  solve_self_sufficient_columns(grid_copy, column_counts, reconfiguration,
                                column_is_solved);

  struct DelayedMoves delayed_moves = {
      .array = malloc(grid_copy->width * sizeof(struct ColumnPair)),
      .length = 0,
  };

  struct ColumnPair best_column_pair =
      column_pair_get_best(grid_copy, column_counts, column_is_solved);
  while (best_column_pair.receiver_index != -1 &&
         best_column_pair.donor_index != -1) {
    solve_column_pair(grid_copy, reconfiguration, &delayed_moves, column_counts,
                      column_is_solved, best_column_pair);

    best_column_pair =
        column_pair_get_best(grid_copy, column_counts, column_is_solved);
  }

  free(column_counts);
  free(column_is_solved);
  free(delayed_moves.array);
  grid_free(grid_copy);
  return reconfiguration;
}
