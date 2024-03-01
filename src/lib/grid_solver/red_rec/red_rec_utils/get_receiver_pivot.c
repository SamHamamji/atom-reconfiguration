#include <assert.h>
#include <stdlib.h>

#include "../../common/source_locations.h"
#include "./red_rec_utils.h"

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

int get_receiver_pivot(const struct Grid *grid,
                       const struct DelayedMoves *delayed_moves,
                       struct Range target_range, struct ColumnPair column_pair,
                       const struct LinearSolver *linear_solver) {
  struct Point *receiver_alias = get_receiver_squished_sources_alias(
      grid, delayed_moves, target_range, column_pair);

  struct Mapping *mapping = linear_solver->solve(
      &(struct Interval){.array = receiver_alias, .length = grid->height},
      linear_solver->params);

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