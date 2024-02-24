#pragma once

#include "../../grid/grid.h"
#include "delayed_moves.h"

struct SourceLocations {
  int upper_reservoir;
  int target_region;
  int lower_reservoir;
};

struct SourceLocations get_column_source_locations(const struct Grid *grid,
                                                   int column_index);
struct SourceLocations
get_delayed_moves_source_locations(const struct Grid *grid,
                                   const struct DelayedMoves *delayed_moves,
                                   int receiver_column_index);
