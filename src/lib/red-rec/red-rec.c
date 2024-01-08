#include <limits.h>
#include <math.h>
#include <stdbool.h>
#include <stdlib.h>

#include "../grid/grid.h"
#include "../linear_solver/linear_solver.h"
#include "../utils/max_min.h"
#include "./red-rec.h"
#include "move.h"
#include "reconfiguration.h"

struct ColumnPair {
  int donor_index;
  int donor_surplus;
  int receiver_index;
  int receiver_deficit;
};

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
  struct SourceLocations column_source_locations = {
      .upper_reservoir = 0,
      .target_region = 0,
      .lower_reservoir = 0,
  };
  int i = 0;
  struct Point *column = grid_get_column(grid, column_index);
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

// Returns a positive number if the first element is greater than the second,
// and vice versa
static int compare_column_pairs(struct ColumnPair a, struct ColumnPair b) {
  /**
   * Comparisons by priority
   * 1. maximizes the number of exchanged tokens
   * 2. minimizes the number of columns between the donor and the receiver
   * 3. maximizes the receiver surplus (closest to saturation)
   */
  int exchanged_sources_a = min(a.donor_surplus, a.receiver_deficit);
  int exchanged_sources_b = min(b.donor_surplus, b.receiver_deficit);
  if (exchanged_sources_a != exchanged_sources_b) {
    return exchanged_sources_a - exchanged_sources_b;
  }

  int column_distance_a = abs(a.donor_index - a.receiver_index);
  int column_distance_b = abs(b.donor_index - b.receiver_index);
  if (column_distance_a != column_distance_b) {
    return column_distance_b - column_distance_a;
  }

  return b.receiver_deficit - a.receiver_deficit;
}

// Returns the best column pair, ensuring that the pair only contains solved
// columns in between the donor and the receiver
static struct ColumnPair get_best_column_pair(struct Grid *grid,
                                              int *column_surpluses,
                                              bool *column_is_solved) {
  int previous_column = -1;
  struct ColumnPair best_column_pair = {
      .receiver_index = -1,
      .donor_index = -1,
      .donor_surplus = 0,
      .receiver_deficit = 0,
  };

  for (int current_column = 0; current_column < grid->width; current_column++) {
    if (column_is_solved[current_column] &&
        column_surpluses[current_column] == 0) {
      continue;
    }
    bool previous_column_is_receiver = column_surpluses[previous_column] < 0;
    bool current_column_is_receiver = column_surpluses[current_column] < 0;
    if (previous_column != -1 &&
        previous_column_is_receiver != current_column_is_receiver) {
      struct ColumnPair current_column_pair = (struct ColumnPair){
          .donor_index =
              previous_column_is_receiver ? current_column : previous_column,
          .receiver_index =
              previous_column_is_receiver ? previous_column : current_column,
      };
      current_column_pair.donor_surplus =
          column_surpluses[current_column_pair.donor_index];
      current_column_pair.receiver_deficit =
          abs(column_surpluses[current_column_pair.receiver_index]);
      if (compare_column_pairs(current_column_pair, best_column_pair) > 0) {
        best_column_pair = current_column_pair;
      }
    }
    previous_column = current_column;
  }
  return best_column_pair;
}

static void
solve_self_sufficient_columns(struct Grid *grid, int *column_supluses,
                              struct Reconfiguration *reconfiguration,
                              bool *column_is_solved) {
  for (int column_index = 0; column_index < grid->width; column_index++) {
    if (column_supluses[column_index] >= 0) {
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
    struct ColumnPair column_pair, struct Range *target_region_range,
    struct Point *receiver_alias) {
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);
  struct SourceLocations donor_source_locations =
      get_column_source_locations(grid, column_pair.donor_index);
  target_region_range->start -= donor_source_locations.upper_reservoir;
  target_region_range->exclusive_end += donor_source_locations.lower_reservoir;

  for (int j = 0; j < grid->height; j++) {
    receiver_alias[j] = (struct Point){
        .is_source = receiver[j].is_source ||
                     (donor[j].is_source && !donor[j].is_target),
        .is_target = target_region_range->start <= j &&
                     j < target_region_range->exclusive_end &&
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
      move_apply(grid, move);
    }
  }
  reconfiguration_add_mapping(reconfiguration, grid, mapping,
                              column_pair.receiver_index);
  mapping_free(mapping);
}

/**  Let (Ci, Cj) denote the best donor-receiver pair;
 *  Let Ci be the receiver column (negative surplus);
 *  Let Cj be the donor column for Ci;
 *  if σj − σi > 0 then
 *    Reconfigure σi tokens from Cj to Ci;
 *    Solve the receiver column Ci (exact 1D);
 * else if σj − σi = 0 then
 *   Reconfigure tokens from Cj to Ci;
 *   Solve the receiver and donor columns (exact 1D);
 * else
 *   Solve the donor column Cj (exact 1D);
 *   Mark excess tokens in Cj for delayed moving to Ci;
 * end if
 * Update surpluses and pairs;
 * Write delayed moves if/when possible;
 */
static void solve_column_pair(struct Grid *grid, int *column_surpluses,
                              struct ColumnPair column_pair,
                              struct Reconfiguration *reconfiguration,
                              struct DelayedMoves *delayed_moves,
                              bool *column_is_solved) {

  int redistributed_atoms_count =
      min(column_pair.donor_surplus, column_pair.receiver_deficit);
  column_surpluses[column_pair.donor_index] -= redistributed_atoms_count;
  column_surpluses[column_pair.receiver_index] += redistributed_atoms_count;

  if (column_pair.donor_surplus < column_pair.receiver_deficit) {
    delayed_moves->array[delayed_moves->length] = column_pair;
    delayed_moves->length++;
    return;
  }

  // Count upper & lower sources
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);
  struct IntervalCounts receiver_counts = {0, 0};
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
  redistributed_sources_locations.lower_reservoir =
      min(redistributed_sources_locations.lower_reservoir,
          receiver_counts.target_num - receiver_counts.source_num -
              redistributed_sources_locations.upper_reservoir);

  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));

  struct Range target_region_range = {
      .start = INT_MAX,
      .exclusive_end = INT_MIN,
  };
  for (int i = 0, target_count = 0; i < grid->height; i++) {
    receiver_alias[i] = (struct Point){
        .is_source = receiver[i].is_source,
        .is_target =
            receiver[i].is_target &&
            redistributed_sources_locations.upper_reservoir <= target_count &&
            target_count < receiver_counts.source_num +
                               redistributed_sources_locations.upper_reservoir,
    };
    if (receiver[i].is_target) {
      if (redistributed_sources_locations.upper_reservoir <= target_count &&
          target_count < receiver_counts.source_num +
                             redistributed_sources_locations.upper_reservoir) {
        target_region_range = (struct Range){
            .start = min(target_region_range.start, i),
            .exclusive_end = max(target_region_range.exclusive_end, i + 1),
        };
      }
      target_count++;
    }
  }

  struct Mapping *mapping = linear_solve_aggarwal(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      NULL);

  reconfiguration_add_mapping(reconfiguration, grid, mapping,
                              column_pair.receiver_index);

  for (int i = reconfiguration_get_move_count(reconfiguration) -
               mapping->pair_count;
       i < reconfiguration_get_move_count(reconfiguration); i++) {
    receiver_alias[reconfiguration_get_move(reconfiguration, i).origin.row]
        .is_source = false;
    receiver_alias[reconfiguration_get_move(reconfiguration, i).destination.row]
        .is_source = true;
  }
  mapping_free(mapping);

  // Solve delayed movements
  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      redistribute_and_reconfigure(grid, reconfiguration,
                                   delayed_moves->array[i],
                                   &target_region_range, receiver_alias);
    }
  }

  redistribute_and_reconfigure(grid, reconfiguration, column_pair,
                               &target_region_range, receiver_alias);

  column_is_solved[column_pair.receiver_index] = true;
  free(receiver_alias);
}

struct Reconfiguration *red_rec(const struct Grid *grid) {
  int *column_surpluses = grid_get_column_supluses(grid);

  int total_surplus = 0;
  for (int i = 0; i < grid->width; i++) {
    total_surplus += column_surpluses[i];
  }

  if (total_surplus < 0) {
    free(column_surpluses);
    return NULL;
  }

  struct Grid *grid_copy = grid_get_copy(grid);
  struct Reconfiguration *reconfiguration =
      reconfiguration_new(2 * grid_copy->width * grid_copy->height);

  bool *column_is_solved = calloc(grid_copy->width, sizeof(bool));

  solve_self_sufficient_columns(grid_copy, column_surpluses, reconfiguration,
                                column_is_solved);

  struct DelayedMoves delayed_moves = {
      .array = malloc(grid_copy->width * sizeof(struct ColumnPair)),
      .length = 0,
  };

  struct ColumnPair best_column_pair =
      get_best_column_pair(grid_copy, column_surpluses, column_is_solved);
  while (best_column_pair.receiver_index != -1 &&
         best_column_pair.donor_index != -1) {

    solve_column_pair(grid_copy, column_surpluses, best_column_pair,
                      reconfiguration, &delayed_moves, column_is_solved);
    best_column_pair =
        get_best_column_pair(grid_copy, column_surpluses, column_is_solved);
  }

  free(column_surpluses);
  free(column_is_solved);
  free(delayed_moves.array);
  grid_free(grid_copy);
  return reconfiguration;
}
