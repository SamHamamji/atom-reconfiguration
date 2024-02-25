#pragma once

#include <stdbool.h>

#include "../../grid/grid.h"

struct ColumnPair {
  int donor_index;
  int receiver_index;
  int exchanged_sources_num;
  int receiver_deficit;
};

bool column_pair_exists(struct ColumnPair pair);

/**
 * Returns the best column pair, making sure that the pair only contains
 * solved columns between the donor and the receiver
 *
 * If no redistribution can be done anymore, returns a nonexistent pair
 */
struct ColumnPair column_pair_get_best(const struct Grid *grid,
                                       const struct Counts *column_counts);
