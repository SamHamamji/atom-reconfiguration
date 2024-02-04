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

static struct SourceLocations
get_column_source_locations(const struct Grid *grid, int column_index) {
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

static struct SourceLocations
get_delayed_moves_source_locations(const struct Grid *grid,
                                   const struct DelayedMoves *delayed_moves,
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

static void solve_self_sufficient_columns(
    struct Grid *grid, const struct Counts *column_counts,
    struct Reconfiguration *reconfiguration, bool *column_is_solved) {
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

static void execute_delayed_move(struct Grid *grid,
                                 struct Reconfiguration *reconfiguration,
                                 struct ColumnPair column_pair,
                                 struct Range fixed_sources_range) {
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);

  struct Mapping *vertical_mapping =
      mapping_new(column_pair.exchanged_sources_num);

  int upper_sources = 0;
  for (int i = fixed_sources_range.start - 1; i >= 0; i--) {
    if (!receiver[fixed_sources_range.start - upper_sources - 1].is_target) {
      break;
    }

    if (donor[i].is_source && !donor[i].is_target) {
      struct Move horizontal_move = {
          .origin = {.col = column_pair.donor_index, .row = i},
          .destination = {.col = column_pair.receiver_index, .row = i},
      };
      reconfiguration_add_move(reconfiguration, horizontal_move);
      grid_apply_move(grid, horizontal_move);

      vertical_mapping->pairs[upper_sources] = (struct Pair){
          .source = i,
          .target = fixed_sources_range.start - upper_sources - 1,
      };
      upper_sources++;
    }
  }

  int lower_sources = 0;
  for (int i = fixed_sources_range.exclusive_end; i < grid->height; i++) {
    if (!receiver[fixed_sources_range.exclusive_end + lower_sources]
             .is_target) {
      break;
    }

    if (donor[i].is_source && !donor[i].is_target) {
      struct Move horizontal_move = {
          .origin = {.col = column_pair.donor_index, .row = i},
          .destination = {.col = column_pair.receiver_index, .row = i},
      };
      reconfiguration_add_move(reconfiguration, horizontal_move);
      grid_apply_move(grid, horizontal_move);

      vertical_mapping->pairs[upper_sources + lower_sources] = (struct Pair){
          .source = i,
          .target = fixed_sources_range.exclusive_end + lower_sources,
      };
      lower_sources++;
    }
  }

  reconfiguration_add_mapping(reconfiguration, grid, vertical_mapping,
                              column_pair.receiver_index);

  mapping_free(vertical_mapping);
}

static void reconfigure_receiver_sources(
    struct Grid *grid, struct Reconfiguration *reconfiguration,
    int receiver_index, struct Range target_region_range) {
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

  struct Mapping *mapping = linear_solve_aggarwal(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      NULL);
  reconfiguration_add_mapping(reconfiguration, grid, mapping, receiver_index);
  mapping_free(mapping);
  free(receiver_alias);
}

static struct Range
get_receiver_sources_target_range(const struct Grid *grid,
                                  const struct DelayedMoves *delayed_moves,
                                  struct ColumnPair column_pair) {
  struct Point *receiver = grid_get_column(grid, column_pair.receiver_index);
  struct Point *donor = grid_get_column(grid, column_pair.donor_index);
  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));

  struct SourceLocations delayed_moves_sources =
      get_delayed_moves_source_locations(grid, delayed_moves, column_pair);

  struct SourceLocations receiver_sources =
      get_column_source_locations(grid, column_pair.receiver_index);

  receiver_sources.upper_reservoir += delayed_moves_sources.upper_reservoir;
  receiver_sources.lower_reservoir += delayed_moves_sources.lower_reservoir;

  struct Range target_range = {
      .start = INT_MAX,
      .exclusive_end = INT_MIN,
  };

  for (int i = 0; i < grid->height; i++) {
    receiver_alias[i] = (struct Point){
        .is_source = (donor[i].is_source && !donor[i].is_target) ||
                     (receiver[i].is_source && receiver[i].is_target),
        .is_target = receiver[i].is_target,
    };
    target_range = (struct Range){
        .start = min(target_range.start, (receiver[i].is_target) ? i : INT_MAX),
        .exclusive_end = max(target_range.exclusive_end,
                             (receiver[i].is_target) ? i + 1 : INT_MIN),
    };
  }

  for (int i = target_range.start; i < target_range.exclusive_end; i++) {
    if (receiver_sources.upper_reservoir == 0) {
      break;
    }
    if (!receiver_alias[i].is_source) {
      receiver_alias[i].is_source = true;
      receiver_sources.upper_reservoir--;
      receiver_sources.target_region++;
    }
  }

  for (int i = target_range.exclusive_end - 1; i >= target_range.start; i--) {
    if (receiver_sources.lower_reservoir == 0) {
      break;
    }
    if (!receiver_alias[i].is_source) {
      receiver_alias[i].is_source = true;
      receiver_sources.lower_reservoir--;
      receiver_sources.target_region++;
    }
  }

  assert(receiver_sources.upper_reservoir == 0);
  assert(receiver_sources.lower_reservoir == 0);

  struct Mapping *mapping = linear_solve_aggarwal(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      NULL);

  int donor_upper_included_sources = 0;
  for (int i = 0; i < mapping->pair_count; i++) {
    if (mapping->pairs[i].source >= mapping->pairs[0].target) {
      break;
    }
    donor_upper_included_sources++;
  }

  free(receiver_alias);
  mapping_free(mapping);

  return (struct Range){
      .start = target_range.start + donor_upper_included_sources +
               delayed_moves_sources.upper_reservoir,
      .exclusive_end = target_range.start + donor_upper_included_sources -
                       delayed_moves_sources.lower_reservoir +
                       receiver_sources.target_region,
  };
}

static void solve_receiver(struct Grid *grid,
                           struct Reconfiguration *reconfiguration,
                           struct DelayedMoves *delayed_moves,
                           struct ColumnPair column_pair) {
  struct Range target_range =
      get_receiver_sources_target_range(grid, delayed_moves, column_pair);

  reconfigure_receiver_sources(grid, reconfiguration,
                               column_pair.receiver_index, target_range);

  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == column_pair.receiver_index) {
      struct SourceLocations source_locations = get_column_source_locations(
          grid, delayed_moves->array[i].donor_index);
      execute_delayed_move(grid, reconfiguration, delayed_moves->array[i],
                           target_range);
      target_range.start -= source_locations.upper_reservoir;
      target_range.exclusive_end += source_locations.lower_reservoir;
    }
  }

  execute_delayed_move(grid, reconfiguration, column_pair, target_range);
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

    if (receiver_deficit != best_pair.exchanged_sources_num) {
      delayed_moves_add(delayed_moves, best_pair);
    } else {
      solve_receiver(copy, reconfiguration, delayed_moves, best_pair);
      column_is_solved[best_pair.receiver_index] = true;
    }

    column_counts[best_pair.donor_index].source_num -=
        best_pair.exchanged_sources_num;
    column_counts[best_pair.receiver_index].source_num +=
        best_pair.exchanged_sources_num;

    best_pair = column_pair_get_best(copy, column_counts, column_is_solved);
  }

  reconfiguration_filter_identical(reconfiguration);

  free(column_counts);
  free(column_is_solved);
  delayed_moves_free(delayed_moves);
  grid_free(copy);
  return reconfiguration;
}
