#include <assert.h>

#include "source_locations.h"

struct SourceLocations get_column_source_locations(const struct Grid *grid,
                                                   int column_index) {
  struct SourceLocations column_source_locations = {0, 0, 0};
  struct Point *column = grid_get_column(grid, column_index);

  int i = 0;
  for (; i < grid->height && !column[i].is_target; i++) {
    column_source_locations.upper_reservoir += (int)column[i].is_source;
  }
  for (; i < grid->height && column[i].is_target; i++) {
    column_source_locations.target_region += (int)column[i].is_source;
  }
  for (; i < grid->height && !column[i].is_target; i++) {
    column_source_locations.lower_reservoir += (int)column[i].is_source;
  }
  assert(i == grid->height);
  return column_source_locations;
}

struct SourceLocations
get_delayed_moves_source_locations(const struct Grid *grid,
                                   const struct DelayedMoves *delayed_moves,
                                   int receiver_column_index) {
  struct SourceLocations source_locations = {0, 0, 0};

  for (int i = 0; i < delayed_moves->length; i++) {
    if (delayed_moves->array[i].receiver_index == receiver_column_index) {
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
