#include <assert.h>
#include <stdlib.h>

#include "red_rec_utils.h"

struct SourceLocations {
  int upper_reservoir;
  int lower_reservoir;
};

static struct SourceLocations
get_column_source_locations(const struct Grid *grid, int column_index,
                            struct Range target_range) {
  struct SourceLocations column_source_locations = {0, 0};
  struct Point *column = grid_get_column(grid, column_index);

  for (int i = 0; i < target_range.start; i++) {
    column_source_locations.upper_reservoir += (int)column[i].is_source;
  }
  for (int i = target_range.exclusive_end; i < grid->height; i++) {
    column_source_locations.lower_reservoir += (int)column[i].is_source;
  }
  return column_source_locations;
}

/**
 * Returns a column representing the receiver with its sources and the sources
 * from its delayed donors squished into its target region. The last donor's
 * sources are simply moved to its target region.
 * It is meant to decide know which sources from the last donor are meant to be
 * used while minimizing the vertically moved distance.
 */
static struct Point *get_receiver_with_squished_sources_alias(
    const struct Grid *grid, const struct ReceiverDelayedMoves delayed_moves,
    struct Range target_range) {
  int last_donor_index =
      delayed_moves.pairs[delayed_moves.length - 1].donor_index;
  int receiver_index = delayed_moves.pairs[0].receiver_index;
  struct Point *receiver = grid_get_column(grid, receiver_index);
  struct Point *last_donor = grid_get_column(grid, last_donor_index);

  struct Point *receiver_alias = malloc(grid->height * sizeof(struct Point));
  for (int i = 0; i < grid->height; i++) {
    receiver_alias[i] = (struct Point){
        .is_source = (last_donor[i].is_source && !last_donor[i].is_target) ||
                     (receiver[i].is_source && receiver[i].is_target),
        .is_target = receiver[i].is_target,
    };
  }

  struct SourceLocations squished_sources =
      get_column_source_locations(grid, receiver_index, target_range);

  // Get all sources except for last donor
  for (int i = 0; i < delayed_moves.length - 1; i++) {
    struct SourceLocations source_locations = get_column_source_locations(
        grid, delayed_moves.pairs[i].donor_index, target_range);
    squished_sources.upper_reservoir += source_locations.upper_reservoir;
    squished_sources.lower_reservoir += source_locations.lower_reservoir;

    assert(source_locations.lower_reservoir +
               source_locations.upper_reservoir ==
           get_exchange_num(delayed_moves.pairs[i]));
  }

  // Squish the sources in the target range
  struct Range squished_sources_range = target_range;
  for (int i = 0; i < squished_sources.upper_reservoir; i++) {
    while (receiver_alias[squished_sources_range.start].is_source) {
      squished_sources_range.start++;
    }
    receiver_alias[squished_sources_range.start].is_source = true;
    squished_sources_range.start++;
  }

  for (int i = 0; i < squished_sources.lower_reservoir; i++) {
    while (receiver_alias[squished_sources_range.exclusive_end - 1].is_source) {
      squished_sources_range.exclusive_end--;
    }
    receiver_alias[squished_sources_range.exclusive_end - 1].is_source = true;
    squished_sources_range.exclusive_end--;
  }

  assert(squished_sources_range.start < squished_sources_range.exclusive_end);

  return receiver_alias;
}

struct Mapping *get_last_donor_mapping(
    const struct Grid *grid, const struct ReceiverDelayedMoves delayed_moves,
    struct Range target_range, const struct LinearSolver *linear_solver) {
  struct Point *receiver_alias = get_receiver_with_squished_sources_alias(
      grid, delayed_moves, target_range);

  struct Mapping *mapping = linear_solver->solve(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      linear_solver->params);

  free(receiver_alias);
  return mapping;
}

int get_pivot_from_mapping(const struct Mapping *mapping,
                           struct Range target_range) {
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

  return pivot;
}

int get_receiver_pivot(const struct Grid *grid,
                       const struct ReceiverDelayedMoves delayed_moves,
                       struct Range target_range,
                       const struct LinearSolver *linear_solver) {
  struct Mapping *mapping =
      get_last_donor_mapping(grid, delayed_moves, target_range, linear_solver);

  int pivot = get_pivot_from_mapping(mapping, target_range);

  mapping_free(mapping);

  return pivot;
}
